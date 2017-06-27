

#include <stdint.h>
#include <susanow.h>
#include <ssn_sys.h>
#include <ssn_types.h>
#include <ssn_green_thread.h>
#include <ssn_native_thread.h>

#include <lthread.h>
#include <dlfcn.h>
#include <vector>
#include <mutex>
#include <queue>
#include <slankdev/extra/dpdk.h>


class ssn_green_thread_manager;
ssn_green_thread_manager* slm[RTE_MAX_LCORE];

using auto_lock=std::lock_guard<std::mutex>;
static void _ssn_thread_spawner(void* arg);
static void _green_thread_gc(void* arg);
static void _green_thread_master_spawner(void* arg);

class ssn_green_thread {
 public:
  lthread* lt;
  ssn_function_t f;
  void* arg;
  size_t lcore_id;
  bool dead;
  ssn_green_thread(ssn_function_t _f, void* _arg, size_t _lcore_id)
    : lt(nullptr), f(_f), arg(_arg), lcore_id(_lcore_id), dead(false)
  { lthread_create(&lt, lcore_id, _ssn_thread_spawner, this); }
  virtual ~ssn_green_thread() {}
};

class ssn_green_thread_manager {
  friend void _green_thread_master_spawner(void* arg);
  friend void _green_thread_gc(void* arg);
 private:
  size_t lcore_id;
  std::vector<ssn_green_thread*> threads;
  bool gc_running;
  mutable std::mutex m;
 public:
  ssn_green_thread_manager(size_t i) : lcore_id(i), gc_running(false) {}
  virtual ~ssn_green_thread_manager() { }
  void sched_register();
  void sched_unregister();
  void debug_dump(FILE* fp);
  void launch(ssn_function_t f, void* arg);
};

void ssn_green_thread_manager::sched_register()
{
  ssn_native_thread_launch(_green_thread_master_spawner, this, lcore_id);
  ssn_set_lcore_state(SSN_LS_RUNNING_GREEN, lcore_id);
}

void ssn_green_thread_manager::sched_unregister()
{
  lthread_scheduler_force_shutdown(lcore_id);
  ssn_set_lcore_state(SSN_LS_FINISHED, lcore_id);
  std::vector<ssn_green_thread*>& vec = threads;
  while (!vec.empty()) {
    ssn_green_thread* sl = vec.back();
    vec.pop_back();
    delete sl;
  }
  gc_running = false;
}

void ssn_green_thread_manager::launch(ssn_function_t f, void* arg)
{
  auto_lock lg(m);
  ssn_green_thread* sl = new ssn_green_thread(f, arg, lcore_id);
  threads.push_back(sl);
}

void ssn_green_thread_manager::debug_dump(FILE* fp)
{
  auto_lock lg(m);
  if (!is_green_thread(lcore_id))
    throw slankdev::exception("is not green thread");

  std::vector<ssn_green_thread*>& vec = threads;
  fprintf(fp, "green_thread lcore%zd \r\n", lcore_id);
  fprintf(fp, " %5s: %-15s %-15s(%-15s) %-15s %-15s\r\n",
      "idx", "lthread_t", "funcptr", "name", "arg", "dead");
  fprintf(fp, " --------------------------------------");
  fprintf(fp, "-----------------------------------\r\n");
  for (size_t i=0; i<vec.size(); i++) {
    Dl_info dli;
    dladdr((void*)vec[i]->f, &dli);
    fprintf(fp, " [%3zd]: %-15p %-15p(%-15s) %-15p %-15s\r\n", i, vec[i]->lt,
        vec[i]->f,  dli.dli_sname, vec[i]->arg, vec[i]->dead?"true":"false");
  }
}

static void _ssn_thread_spawner(void* arg)
{
  ssn_green_thread* sl = reinterpret_cast<ssn_green_thread*>(arg);
  sl->f(sl->arg);
  sl->dead = true;
}

static void _green_thread_master_spawner(void* arg)
{
  ssn_green_thread_manager* mgr = reinterpret_cast<ssn_green_thread_manager*>(arg);
  mgr->gc_running = true;

  ssn_green_thread* sl_gc = new ssn_green_thread(_green_thread_gc  , arg    , mgr->lcore_id);
  lthread_run();
  delete sl_gc;
}

static void _green_thread_gc(void* arg)
{
  ssn_green_thread_manager* mgr = reinterpret_cast<ssn_green_thread_manager*>(arg);
  std::vector<ssn_green_thread*>& vec = mgr->threads;

  while (mgr->gc_running) {
    ssn_sleep(1000);
    auto_lock lg(mgr->m);
    size_t nb_th = vec.size();
    for (size_t i=0; i<nb_th; i++) {
      if (vec[i]->dead) {
        ssn_green_thread* sl = vec[i];
        vec.erase(vec.begin() + i);
        delete sl;
        break;
      }
    }
  }
}


/*
 * SSN APIs are below
 */

void ssn_green_thread_init()
{
  size_t nb = rte_lcore_count();
  for (size_t i=0; i<nb; i++) slm[i] = new ssn_green_thread_manager(i);
}
void ssn_green_thread_fin()
{
  size_t nb = rte_lcore_count();
  for (size_t i=0; i<nb; i++) delete slm[i];
}
void ssn_green_thread_launch(ssn_function_t f, void* arg, size_t lcore_id) { slm[lcore_id]->launch(f, arg); }
void ssn_green_thread_debug_dump(FILE* fp, size_t lcore_id) { slm[lcore_id]->debug_dump(fp); }
void ssn_green_thread_sched_register(size_t lcore_id) { slm[lcore_id]->sched_register(); }
void ssn_green_thread_sched_unregister(size_t lcore_id) { slm[lcore_id]->sched_unregister(); }


