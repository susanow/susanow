
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <memory>

#include <ssn_port.h>
#include <ssn_log.h>
#include <ssn_cpu.h>
#include <ssn_common.h>
#include <ssn_thread.h>
#include <ssn_port_stat.h>

#include <dpdk/dpdk.h>
#include <slankdev/exception.h>

#include <ssn_vnf_port.h>

/*-------------------User-Code-Below-------------------------*/

size_t num[] = {0,1,2,3,4,5,6,7,8};

bool l2fwd_running = true;
void l2fwd(void* acc_id_)
{
  size_t aid = *((size_t*)acc_id_);
  ssn_log(SSN_LOG_INFO, "start new thread %s, access_id=%zd\n", __func__, aid);

  size_t nb_ports = ssn_dev_count();
  while (l2fwd_running) {
    for (size_t pid=0; pid<nb_ports; pid++) {
      rte_mbuf* mbufs[32];
      size_t nb_recv = ssn_vnf_port_rx_burst(pid, aid, mbufs, 32);
      if (nb_recv == 0) continue;

      for (size_t i=0; i<nb_recv; i++) {

        /* Delay Block begin */
        size_t n=10;
        for (size_t j=0; j<100; j++) n++;

        size_t nb_send = ssn_vnf_port_tx_burst(pid^1, aid, &mbufs[i], 1);
        if (nb_send != 1)
          rte_pktmbuf_free(mbufs[i]);
      }
    }
  } /* while */

  ssn_log(SSN_LOG_INFO, "finish thread %s \n", __func__);
}

void INIT(int argc, char** argv, size_t n_que)
{
  ssn_init(argc, argv);

  size_t n_ports = ssn_dev_count();
  if (n_ports != 2) throw slankdev::exception("num ports is not 2");
  for (size_t i=0; i<n_ports; i++) {
    ssn_vnf_port_configure_hw(i, n_que, n_que);
    ssn_vnf_port_dev_up(i);
    ssn_vnf_port_promisc_on(i);
  }

  if (n_ports != 2) {
    std::string err = slankdev::format("num ports is not 2 (current %zd)",
        ssn_dev_count());
    throw slankdev::exception(err.c_str());
  }
}

int main(int argc, char** argv)
{
  uint32_t tid[4];
  INIT(argc, argv, 4);

  getchar();
  ssn_vnf_port_configure_acc(0, 1, 1);
  ssn_vnf_port_configure_acc(1, 1, 1);
  l2fwd_running = true;
  tid[0] = ssn_thread_launch(l2fwd, &num[0], 1);

  getchar();
  l2fwd_running = false;
  ssn_thread_join(tid[0]);
  ssn_vnf_port_configure_acc(0, 2, 2);
  ssn_vnf_port_configure_acc(1, 2, 2);
  l2fwd_running = true;
  tid[0] = ssn_thread_launch(l2fwd, &num[0], 1);
  tid[1] = ssn_thread_launch(l2fwd, &num[1], 2);

  getchar();
  l2fwd_running = false;
  ssn_thread_join(tid[0]);
  ssn_thread_join(tid[1]);
  ssn_vnf_port_configure_acc(0, 4, 4);
  ssn_vnf_port_configure_acc(1, 4, 4);
  l2fwd_running = true;
  tid[0] = ssn_thread_launch(l2fwd, &num[0], 1);
  tid[1] = ssn_thread_launch(l2fwd, &num[1], 2);
  tid[2] = ssn_thread_launch(l2fwd, &num[2], 3);
  tid[3] = ssn_thread_launch(l2fwd, &num[3], 4);

  getchar();
  l2fwd_running = false;
  ssn_thread_join(tid[0]);
  ssn_thread_join(tid[1]);
  ssn_thread_join(tid[2]);
  ssn_thread_join(tid[3]);
  ssn_fin();
}


