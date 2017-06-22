
#pragma once
#include <ssn_types.h>

void ssn_lthread_init();
void ssn_lthread_fin();
void ssn_lthread_sched_register(size_t lcore_id);
void ssn_lthread_sched_unregister(size_t lcore_id);
void ssn_lthread_launch(ssn_function_t f, void* arg, size_t lcore_id);
void ssn_lthread_debug_dump(FILE* fp, size_t lcore_id);
void ssn_lthread_debug_dump(FILE* fp);



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


class ssn_lthread {
 public:
  lthread* lt;
  ssn_function_t f;
  void* arg;
  size_t lcore_id;
  ssn_lthread(ssn_function_t _f, void* _arg, size_t _lcore_id)
    : lt(nullptr), f(_f), arg(_arg), lcore_id(_lcore_id)
  { lthread_create(&lt, lcore_id, f, arg); }
  virtual ~ssn_lthread() {}
};

class ssn_lthread_manager;
ssn_lthread_manager* slm[RTE_MAX_LCORE];

void _lthread_null(void*) {}
void _lthread_master_spawner(void* arg)
{
  struct lthread* lt = nullptr;
  lthread_create(&lt, -1, _lthread_null, nullptr);
  lthread_run();
}

class ssn_lthread_manager {
 private:
  size_t lcore_id;
  std::vector<ssn_lthread*> lthreads;
 public:
  ssn_lthread_manager(size_t i) : lcore_id(i) {}
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


void ssn_lthread_manager::sched_register()
{
  ssn_launch(_lthread_master_spawner, this, lcore_id);
  sys.cpu.lcores[lcore_id].state = SSN_LS_RUNNING_LTHREAD;
}

void ssn_lthread_manager::sched_unregister()
{
  lthread_scheduler_force_shutdown(lcore_id);
  sys.cpu.lcores[lcore_id].state = SSN_LS_FINISHED;
  std::vector<ssn_lthread*>& vec = lthreads;
  while (!vec.empty()) {
    ssn_lthread* sl = vec.back();
    vec.pop_back();
    delete sl;
  }
}

void ssn_lthread_manager::launch(ssn_function_t f, void* arg)
{
  ssn_lthread* sl = new ssn_lthread(f, arg, lcore_id);
  lthreads.push_back(sl);
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



