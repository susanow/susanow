

#include <unistd.h>

#include <lthread.h>
#include <slankdev/extra/dpdk.h>

#include <ssn_cpu.h>
#include <ssn_common.h>
#include <ssn_types.h>
#include <ssn_timer.h>
#include <ssn_vty.h>
#include <ssn_rest.h>
#include <ssn_green_thread.h>
#include <ssn_native_thread.h>
#include <ssn_log.h>
#include <ssn_port.h>
#include <ssn_port_stat.h>

void ssn_init(int argc, char** argv)
{
  ssn_cpu_init(argc, argv);
  ssn_native_thread_init();
  ssn_green_thread_init();
  ssn_timer_init();
  ssn_port_init();
  ssn_port_stat_init();
}
void ssn_fin()
{
  ssn_native_thread_fin();
  ssn_green_thread_fin();
  ssn_timer_fin();
  ssn_port_fin();
  ssn_port_stat_fin();
}
void ssn_wait_all_lcore()
{
  ssn_log(SSN_LOG_INFO, "wait all lcore\n");
  rte_eal_mp_wait_lcore();
  ssn_log(SSN_LOG_INFO, "all lcore was joined\n");
}


