

#include <susanow.h>
#include <ssn_lthread.h>
#include <ssn_sys.h>
#include <ssn_types.h>

#include <lthread.h>
#include <dlfcn.h>
#include <vector>
#include <queue>
#include <slankdev/extra/dpdk.h>

class ssn_lthread_manager;
ssn_lthread_manager* slm[RTE_MAX_LCORE];


class ssn_lthread {
 public:
  lthread* lt;
  ssn_function_t f;
  void* arg;
  bool dead;
  ssn_lthread(ssn_function_t _f, void* _arg)
    : lt(nullptr), f(_f), arg(_arg), dead(false) {}
  virtual ~ssn_lthread() {}
  void create() { lthread_create(&lt, f, arg); }
};

class ssn_lthread_manager {
  friend void _lthread_control(void* arg);
  friend void _lthread_start(void* arg);
 private:
  size_t lcore_id;
  bool lthread_running;
  ssn_lthread* starter;
  std::vector<ssn_lthread*> lthreads;
  std::queue<ssn_lthread*>  pre_launch_lthreads;
 public:
  ssn_lthread_manager(size_t i) : lcore_id(i), lthread_running(false), starter(nullptr) {}
  virtual ~ssn_lthread_manager() {}
  void sched_register();
  void sched_unregister();
  void debug_dump(FILE* fp);
  void launch(ssn_function_t f, void* arg);

};

void ssn_lthread_init()
{
  size_t nb = rte_lcore_count();
  for (size_t i=0; i<nb; i++) {
    slm[i] = new ssn_lthread_manager(i);
  }
}
void ssn_lthread_fin()
{
  size_t nb = rte_lcore_count();
  for (size_t i=0; i<nb; i++) {
    delete slm[i];
  }
}
void ssn_lthread_launch(ssn_function_t f, void* arg, size_t lcore_id) { slm[lcore_id]->launch(f, arg); }
void ssn_lthread_debug_dump(FILE* fp, size_t lcore_id) { slm[lcore_id]->debug_dump(fp); }
void ssn_lthread_sched_register(size_t lcore_id) { slm[lcore_id]->sched_register(); }
void ssn_lthread_sched_unregister(size_t lcore_id) { slm[lcore_id]->sched_unregister(); }

void _lthread_control(void* arg)
{
  ssn_lthread_manager* mgr = reinterpret_cast<ssn_lthread_manager*>(arg);

  std::queue<ssn_lthread*>&  pllts = mgr->pre_launch_lthreads;
  std::vector<ssn_lthread*>& lts   = mgr->lthreads;

  mgr->lthread_running = true;
  while (mgr->lthread_running) {
    /*
     * Check and Launch
     */
    while (!pllts.empty()) {
      ssn_lthread* lt = pllts.front();
      pllts.pop();
      lt->create();
      lts.push_back(lt);
    }

    /*
     * Check and Join
     */
    for (size_t i=0; i<lts.size(); i++) {
      int ret = lthread_join(lts[i]->lt, nullptr, 1);
      if (ret == 0 || ret == -1) {
        lts[i]->dead = true;
      }
    }
    for (size_t i=0; i<lts.size(); i++) {
      if (lts[i]->dead) {
        ssn_lthread* _lt = lts[i];
        delete _lt;
        lts.erase(lts.begin() + i);
        i--;
      }
    }
    lthread_sleep(1);
  } // while
}
void _lthread_start(void* arg)
{
  ssn_lthread_manager* mgr = reinterpret_cast<ssn_lthread_manager*>(arg);
  mgr->starter = new ssn_lthread(_lthread_control, arg);
  mgr->starter->create();
  lthread_run();
  delete mgr->starter;
}

void ssn_lthread_manager::launch(ssn_function_t f, void* arg)
{
  ssn_lthread* sl = new ssn_lthread(f, arg);
  pre_launch_lthreads.push(sl);
}
void ssn_lthread_manager::debug_dump(FILE* fp)
{
  if (!is_lthread(lcore_id)) throw slankdev::exception("is not lthread");

  std::vector<ssn_lthread*>& vec = lthreads;
  fprintf(fp, "lthread lcore%zd \r\n", lcore_id);
  fprintf(fp, " %5s: %-15s %-15s(%-15s) %-15s \r\n",
      "idx", "lthread", "funcptr", "name", "arg");
  fprintf(fp, " --------------------------------------");
  fprintf(fp, "-----------------------------------\r\n");
  for (size_t i=0; i<vec.size(); i++) {
    Dl_info dli;
    dladdr((void*)vec[i]->f, &dli);
    fprintf(fp, " [%3zd]: %-15p %-15p(%-15s) %-15p \r\n", i, vec[i]->lt,
        vec[i]->f,  dli.dli_sname, vec[i]->arg);
  }
}
void ssn_lthread_manager::sched_register()
{
  sys.cpu.lcores[lcore_id].state = SSN_LS_RUNNING_LTHREAD;
  ssn_launch(_lthread_start, this, lcore_id);
}
void ssn_lthread_manager::sched_unregister()
{
  lthread_running = false;

  std::queue<ssn_lthread*>&  que = pre_launch_lthreads;
  while (!que.empty()) {
    ssn_lthread* sl = que.front();
    que.pop();
    delete sl;
  }
  std::vector<ssn_lthread*>& vec = lthreads;
  if (vec.size() != 0)
    throw slankdev::exception("There are threads not freed yet");

  sys.cpu.lcores[lcore_id].state = SSN_LS_FINISHED;
}

