
#include <susanow.h>

void ssn_init(int argc, char** argv)
{
  ssn_sys_init(argc, argv);
  ssn_lthread_init();
}
void ssn_fin()
{
  ssn_lthread_fin();
  rte_eal_mp_wait_lcore();
}

