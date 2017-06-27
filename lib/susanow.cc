

#include <susanow.h>
#include <unistd.h>
#include <lthread.h>
#include <slankdev/extra/dpdk.h>
#include <ssn_native_thread.h>

void ssn_init(int argc, char** argv)
{
  ssn_sys_init(argc, argv);
  ssn_native_thread_init();
  ssn_green_thread_init();
  ssn_timer_init();
  ssn_port_init();
}
void ssn_fin()
{
  rte_eal_mp_wait_lcore();
  ssn_native_thread_fin();
  ssn_green_thread_fin();
  ssn_timer_fin();
  ssn_port_fin();
}


