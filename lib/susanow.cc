

#include <susanow.h>

void ssn_sleep(size_t msec)
{
  if (is_lthread(rte_lcore_id())) lthread_sleep(msec);
  else usleep(msec * 1000);
}
