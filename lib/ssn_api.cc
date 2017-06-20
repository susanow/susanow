

#include <susanow.h>

void ssn_sleep(size_t msec)
{
  if (is_lthread(rte_lcore_id())) lthread_sleep(msec);
  else usleep(msec * 1000);
}

bool is_lthread(size_t lcore_id)
{ return ssn_get_lcore_state(lcore_id) == SSN_LS_RUNNING_LTHREAD; }
bool is_tthread(size_t lcore_id)
{ return ssn_get_lcore_state(lcore_id) == SSN_LS_RUNNING_TIMER; }

