
#include <ssn_sys.h>
#include <ssn_native_thread.h>
#include <susanow.h>
#include <slankdev/extra/dpdk.h>


class ssn_native_thread {
 public:
  ssn_function_t f;
  void*          arg;
};
ssn_native_thread* snt[RTE_MAX_LCORE];

static int _fthread_launcher(void* arg)
{
  size_t lcore_id = rte_lcore_id();
  ssn_native_thread* snt = reinterpret_cast<ssn_native_thread*>(arg);
  ssn_set_lcore_state(SSN_LS_RUNNING_NATIVE, lcore_id);
  snt->f(snt->arg);
  ssn_set_lcore_state(SSN_LS_FINISHED, lcore_id);
  return 0;
}

void ssn_native_thread_launch(ssn_function_t f, void* arg, size_t lcore_id)
{
  snt[lcore_id]->f   = f;
  snt[lcore_id]->arg = arg;
  rte_eal_remote_launch(_fthread_launcher, snt[lcore_id], lcore_id);
}

void ssn_wait(size_t lcore_id)
{
  rte_lcore_state_t s = rte_eal_get_lcore_state(lcore_id);
  if (s == FINISHED) {
    rte_eal_wait_lcore(lcore_id);
    ssn_set_lcore_state(SSN_LS_WAIT, lcore_id);
  }
  if (s == WAIT) {
    ssn_set_lcore_state(SSN_LS_WAIT, lcore_id);
  }
}

void ssn_waiter_thread(void*)
{
  size_t nb_lcores = ssn_lcore_count();
  while (true) {
    for (size_t i=0; i<nb_lcores; i++) {
      ssn_wait(i);
      ssn_sleep(1);
    }
  }
}

void ssn_native_thread_init()
{
  size_t nb_lcores = rte_lcore_count();
  for (size_t i=0; i<nb_lcores; i++) {
    snt[i] = new ssn_native_thread;
  }
}

void ssn_native_thread_fin()
{
  size_t nb_lcores = rte_lcore_count();
  for (size_t i=0; i<nb_lcores; i++) {
    delete snt[i];
  }
}


