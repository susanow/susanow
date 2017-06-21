
#include <susanow.h>

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

