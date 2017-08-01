

#include <stdio.h>
#include <stdio.h>
#include <ssn_log.h>
#include <ssn_port.h>
#include <ssn_sys.h>
#include <ssn_common.h>
#include <ssn_native_thread.h>
#include <ssn_ring.h>

ssn_ring* ring_rx[2];
ssn_ring* ring_tx[2];


bool running = true;
void l2fwd(void*)
{
  size_t nb_ports = ssn_dev_count();
  printf("start l2fwd nb_ports=%zd lcore%zd\n", nb_ports, ssn_lcore_id());
  while (running) {
    for (size_t pid=0; pid<nb_ports; pid++) {
      rte_mbuf* mbufs[32];
      size_t nb_recv = ssn_port_rx_burst(pid, 0, mbufs, 32);
      if (nb_recv == 0) continue;
      size_t nb_enq = ring_rx[pid]->enq_bulk((void**)mbufs, nb_recv);
      if (nb_recv > nb_enq) {
        // printf("aaaaaaaaaaaaaaaaaaaaaaaaaadrop\n");
        ssn_mbuf_free_bulk(&mbufs[nb_enq], nb_recv-nb_enq);
      }
    }
    for (size_t pid=0; pid<nb_ports; pid++) {
      rte_mbuf* mbufs[32];
      size_t nb_deq = ring_tx[pid]->deq_bulk((void**)mbufs, 32);
      if (nb_deq == 0) continue;

      size_t nb_send = ssn_port_tx_burst(pid, 0, mbufs, nb_deq);
      if (nb_deq > nb_send) {
        // printf("dradfdfdfdfdop pid=%zd nb_send=%zd, nb_deq=%zd\n", pid, nb_send, nb_deq);
        ssn_mbuf_free_bulk(&mbufs[nb_send], nb_deq-nb_send);
      }
    }
  }
}

void wk(void*)
{
  size_t nb_ports = ssn_dev_count();
  printf("start wk nb_ports=%zd lcore%zd\n", nb_ports, ssn_lcore_id());
  while (running) {
    for (size_t pid=0; pid<nb_ports; pid++) {
      rte_mbuf* mbufs[32];
      size_t nb_deq = ring_rx[pid]->deq_bulk((void**)mbufs, 32);
      if (nb_deq == 0) continue;
      for (size_t i=0; i<nb_deq; i++) {

        size_t n = 0;
        for (size_t j=0; j<30; j++) n++;

        int ret = ring_tx[pid^1]->enq(mbufs[i]);
        if (ret < 0)
          rte_pktmbuf_free(mbufs[i]);
      }
    }
  }
}

void wait_enter(const char* msg)
{
  printf("%s [Enter]: ", msg);
  getchar();
}

int main(int argc, char** argv)
{
  ssn_init(argc, argv);
  ssn_log_set_level(SSN_LOG_INFO);
  printf("\n\n");

  ring_rx[0] = new ssn_ring("rx0");
  ring_rx[1] = new ssn_ring("rx1");
  ring_tx[0] = new ssn_ring("tx0");
  ring_tx[1] = new ssn_ring("tx1");

  ssn_port_conf conf;
  size_t nb_ports = ssn_dev_count();
  for (size_t i=0; i<nb_ports; i++) {
    ssn_port_configure(i, &conf);
    ssn_port_dev_up(i);
    ssn_port_link_up(i);
    ssn_port_promisc_on(i);
  }

  ssn_native_thread_launch(l2fwd, nullptr, 2);
  ssn_native_thread_launch(wk   , nullptr, 3);
  wait_enter("PushEnterToExitL2fwd");
  running = false;
  ssn_sleep(1000);

  ssn_fin();
}


