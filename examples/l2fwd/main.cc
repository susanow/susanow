

#include <stdio.h>
#include <unistd.h>
#include <slankdev/extra/dpdk.h>
#include <ssn_native_thread.h>
#include <ssn_green_thread.h>
#include <ssn_port.h>
#include <ssn_log.h>
#include <ssn_common.h>
#include <ssn_sys.h>
#include <ssn_port_stat.h>
#define DEBUG


bool running = true;
void wk(void*)
{
  ssn_log(SSN_LOG_INFO, "start rx-thread \n");
  size_t nb_ports = ssn_dev_count();
  while (running) {
    for (size_t pid=0; pid<nb_ports; pid++) {
      rte_mbuf* mbufs[32];
      size_t nb_recv = ssn_port_rx_burst(pid, 0, mbufs, 32);
      if (nb_recv == 0) continue;

#ifdef DEBUG
      printf("recv\n");
      static size_t cnt = 0;
      if (cnt >= 100000) {
        printf("forward %zd packets\n", cnt );
        cnt = 0;
      } else {
        cnt ++;
      }
#endif

      size_t nb_send = ssn_port_tx_burst(pid^1,0, mbufs, nb_recv);
      if (nb_send < nb_recv)
        ssn_mbuf_free_bulk(&mbufs[nb_send], nb_recv-nb_send);
    }
  }
  ssn_log(SSN_LOG_INFO, "finish rx-thread \n");
}

int main(int argc, char** argv)
{
  // ssn_log_set_level(SSN_LOG_EMERG);
  ssn_log_set_level(SSN_LOG_INFO);
  ssn_init(argc, argv);

  ssn_green_thread_sched_register(1);

  size_t nb_ports = ssn_dev_count();
  if (nb_ports != 2) throw slankdev::exception("num ports is not 2");

  printf("----------------------\n");
  ssn_port_conf conf;
  conf.debug_dump(stdout);
  printf("----------------------\n");
  for (size_t i=0; i<nb_ports; i++) {
    ssn_port_configure(i, &conf);
    ssn_port_dev_up(i);
    ssn_port_promisc_on(i);
  }

  ssn_native_thread_launch(wk, nullptr, 2);
  while (1);
  getchar();
  running = false;

  ssn_green_thread_sched_unregister(1);
  ssn_wait_all_lcore();
  ssn_fin();
}


