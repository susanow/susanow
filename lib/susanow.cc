

#include <susanow.h>
#include <unistd.h>
#include <lthread.h>
#include <slankdev/extra/dpdk.h>

void ssn_init(int argc, char** argv)
{
  ssn_sys_init(argc, argv);
  ssn_lthread_init();
  ssn_timer_init();
}
void ssn_fin()
{
  rte_eal_mp_wait_lcore();
  ssn_lthread_fin();
  ssn_timer_fin();
}

void ssn_sleep(size_t msec)
{
  if (is_lthread(rte_lcore_id())) lthread_sleep(msec);
  else usleep(msec * 1000);
}

bool is_lthread(size_t lcore_id) { return ssn_get_lcore_state(lcore_id) == SSN_LS_RUNNING_LTHREAD; }
bool is_tthread(size_t lcore_id) { return ssn_get_lcore_state(lcore_id) == SSN_LS_RUNNING_TIMER; }



