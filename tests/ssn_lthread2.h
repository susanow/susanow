
#pragma once
#include <ssn_types.h>

void ssn_lthread_init2();
void ssn_lthread_fin2();
void ssn_lthread_sched_register2(size_t lcore_id);
void ssn_lthread_sched_unregister2(size_t lcore_id);
void ssn_lthread_launch2(ssn_function_t f, void* arg, size_t lcore_id);
void ssn_lthread_debug_dump2(FILE* fp, size_t lcore_id);
void ssn_lthread_debug_dump2(FILE* fp);



/*
 * Implementation
 */

#include <stdint.h>
#include <susanow.h>
#include <ssn_lthread.h>
#include <ssn_sys.h>
#include <ssn_types.h>

#include <lthread.h>
#include <dlfcn.h>
#include <vector>
#include <queue>
#include <slankdev/extra/dpdk.h>


class ssn_lthread2 {
 public:
  lthread* lt;
  ssn_function_t f;
  void* arg;
  bool dead;
  ssn_lthread2(ssn_function_t _f, void* _arg)
    : lt(nullptr), f(_f), arg(_arg), dead(false) {}
  virtual ~ssn_lthread2() {}
  void create() { lthread_create(&lt, f, arg); }
};



class ssn_lthread_manager2;
ssn_lthread_manager2* slm2[RTE_MAX_LCORE];


class ssn_lthread_manager2 {
  friend void _lthread_control2(void* arg);
  friend void _lthread_start2(void* arg);
 private:
  size_t lcore_id;
  bool lthread_running;
  ssn_lthread2* starter;
  std::vector<ssn_lthread2*> lthreads;
  std::queue<ssn_lthread2*>  pre_launch_lthreads;
 public:
  ssn_lthread_manager2(size_t i) : lcore_id(i), lthread_running(false), starter(nullptr) {}
  virtual ~ssn_lthread_manager2() {}
  void sched_register();
  void sched_unregister();
  void debug_dump(FILE* fp);
  void launch(ssn_function_t f, void* arg);

};

void ssn_lthread_init2()
{
  size_t nb = rte_lcore_count();
  for (size_t i=0; i<nb; i++) {
    slm2[i] = new ssn_lthread_manager2(i);
  }
}
void ssn_lthread_fin2()
{
  size_t nb = rte_lcore_count();
  for (size_t i=0; i<nb; i++) {
    delete slm2[i];
  }
}
void ssn_lthread_launch2(ssn_function_t f, void* arg, size_t lcore_id) { slm2[lcore_id]->launch(f, arg); }
void ssn_lthread_debug_dump2(FILE* fp, size_t lcore_id) { slm2[lcore_id]->debug_dump(fp); }
void ssn_lthread_sched_register2(size_t lcore_id) { slm2[lcore_id]->sched_register(); }
void ssn_lthread_sched_unregister2(size_t lcore_id) { slm2[lcore_id]->sched_unregister(); }

void _lthread_control2(void* arg)
{
  ssn_lthread_manager2* mgr = reinterpret_cast<ssn_lthread_manager2*>(arg);

  std::queue<ssn_lthread2*>&  pllts = mgr->pre_launch_lthreads;
  std::vector<ssn_lthread2*>& lts   = mgr->lthreads;

  mgr->lthread_running = true;
  while (mgr->lthread_running) {
    /*
     * Check and Launch
     */
    while (!pllts.empty()) {
      ssn_lthread2* lt = pllts.front();
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
        ssn_lthread2* _lt = lts[i];
        delete _lt;
        lts.erase(lts.begin() + i);
        i--;
      }
    }
    lthread_sleep(1);
  } // while
}
void _lthread_start2(void* arg)
{
  ssn_lthread_manager2* mgr = reinterpret_cast<ssn_lthread_manager2*>(arg);
  mgr->starter = new ssn_lthread2(_lthread_control2, arg);
  mgr->starter->create();
  lthread_run();
  delete mgr->starter;
}

void ssn_lthread_manager2::launch(ssn_function_t f, void* arg)
{
  ssn_lthread2* sl = new ssn_lthread2(f, arg);
  pre_launch_lthreads.push(sl);
}
void ssn_lthread_manager2::debug_dump(FILE* fp)
{
  if (!is_lthread(lcore_id)) throw slankdev::exception("is not lthread");

  std::vector<ssn_lthread2*>& vec = lthreads;
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
void ssn_lthread_manager2::sched_register()
{
  ssn_launch(_lthread_start2, this, lcore_id);
  sys.cpu.lcores[lcore_id].state = SSN_LS_RUNNING_LTHREAD;
}
void ssn_lthread_manager2::sched_unregister()
{
  lthread_running = false;

  std::queue<ssn_lthread2*>&  que = pre_launch_lthreads;
  while (!que.empty()) {
    ssn_lthread2* sl = que.front();
    que.pop();
    delete sl;
  }
  std::vector<ssn_lthread2*>& vec = lthreads;
  if (vec.size() != 0)
    throw slankdev::exception("There are threads not freed yet");

  sys.cpu.lcores[lcore_id].state = SSN_LS_FINISHED;
}

