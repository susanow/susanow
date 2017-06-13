
#include <lthread.h>
#include <queue>
#include <slankdev/extra/dpdk.h>
#include <slankdev/hexdump.h>
#include "ssn_lthread.h"

void launcher(void*)
{
  ssn_lthread_sched* sched = reinterpret_cast<ssn_lthread_sched*>(lthread_get_data());
  sched->running = true;
  while (sched->running) {
    if (sched->funcs.empty()) {
      lthread_sleep(1);
      continue;
    }

    ssn_lthread_sched::call_info ci = sched->funcs.front();
    sched->funcs.pop();

    lthread* th;
    lthread_create_and_setdata(&th, ci.f, ci.arg, sched);
    lthread_sleep(1);
  }
}

int lthread_thread(void* arg)
{
  lthread* l;
  lthread_create_and_setdata(&l, launcher, nullptr, arg);
  lthread_run();
}

void ssn_lthread_sched::start_scheduler(uint32_t lcore_id)
{ rte_eal_remote_launch(lthread_thread, this, lcore_id); }

void ssn_lthread_sched::stop_scheduler() { running = false; }

void ssn_lthread_sched::launch(ssn_function_t f, void* a) { funcs.push({f, a}); }
ssn_lthread_sched::ssn_lthread_sched() : running(false) {}
ssn_lthread_sched* current_sched()
{
  struct lthread* lt = lthread_current();
  return (ssn_lthread_sched*)lthread_get_data();
}


