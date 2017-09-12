

#include <ssn_cpu.h>
#include <ssn_log.h>
#include <ssn_native_thread.h>
#include <dpdk/dpdk.h>
#include <slankdev/exception.h>


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
  ssn_log(SSN_LOG_DEBUG, "launch native thread to lcore%zd \n", lcore_id);
  size_t n_lcore = rte_lcore_count();
  if (n_lcore <= lcore_id) {
    throw slankdev::exception("too huge lcore_id?");
  }

  snt[lcore_id]->f   = f;
  snt[lcore_id]->arg = arg;
  dpdk::rte_eal_remote_launch(_fthread_launcher, snt[lcore_id], lcore_id);
}

bool ssn_lcore_joinable(size_t lcore_id)
{
  rte_lcore_state_t s = rte_eal_get_lcore_state(lcore_id);
  return s==FINISHED;
}

void ssn_lcore_join(size_t lcore_id)
{
  int ret = rte_eal_wait_lcore(lcore_id);
  UNUSED(ret);
  ssn_set_lcore_state(SSN_LS_WAIT, lcore_id);
  ssn_log(SSN_LOG_DEBUG, "join lcore%zd \n", lcore_id);
}

bool ssn_lcore_join_poll_thread_running;
void ssn_lcore_join_poll_thread_stop()
{ ssn_lcore_join_poll_thread_running=false; }
void ssn_lcore_join_poll_thread(void*)
{
  ssn_lcore_join_poll_thread_running = true;
  size_t lcore_id = ssn_lcore_id();
  while (ssn_lcore_join_poll_thread_running) {
    size_t nb_lcores = ssn_lcore_count();
    for (size_t i=0; i<nb_lcores; i++) {
      if (ssn_lcore_joinable(i)) {
        ssn_lcore_join(i);
      }
    }
    if (is_green_thread(lcore_id)) ssn_yield();
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

