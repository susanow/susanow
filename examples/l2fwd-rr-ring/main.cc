

#include <stdio.h>
#include <stdio.h>
#include <ssn_log.h>
#include <ssn_port.h>
#include <ssn_sys.h>
#include <ssn_common.h>
#include <ssn_native_thread.h>
#include <ssn_ring.h>
#include "ssn_rr_ring.h"
#define DELAY 50

ssn_ring* ring_rx[2][2];
ssn_ring* ring_tx[2][2];

ssn_rr_rx_rings wk0_rx;
ssn_rr_rx_rings wk1_rx;


bool running = true;
void l2fwd(void*)
{
  size_t pivot = 0;
  size_t nb_ports = ssn_dev_count();
  printf("start %s nb_ports=%zd lcore%zd\n", __func__, nb_ports, ssn_lcore_id());
  pivot = 0;
  while (running) {
    for (size_t pid=0; pid<nb_ports; pid++) {
      rte_mbuf* mbufs[32];
      size_t nb_recv = ssn_port_rx_burst(pid, 0, mbufs, 32);

      size_t nb_enq = ring_rx[pid][pivot]->enq_burst((void**)mbufs, nb_recv);
      if (nb_recv > nb_enq) {
        ssn_mbuf_free_bulk(&mbufs[nb_enq], nb_recv-nb_enq);
      }
    }
    for (size_t pid=0; pid<nb_ports; pid++) {
      rte_mbuf* mbufs[32];
      size_t nb_deq = ring_tx[pid][pivot]->deq_burst((void**)mbufs, 32);

      size_t nb_send = ssn_port_tx_burst(pid, 0, mbufs, nb_deq);
      if (nb_deq > nb_send) {
        ssn_mbuf_free_bulk(&mbufs[nb_send], nb_deq-nb_send);
      }
    }
    pivot ^= 1;
  }
}

void wk0(void*)
{
  wk0_rx.add_ring();
  size_t nb_ports = ssn_dev_count();
  printf("start %s nb_ports=%zd lcore%zd\n", __func__, nb_ports, ssn_lcore_id());
  while (running) {
    for (size_t pid=0; pid<nb_ports; pid++) {
      rte_mbuf* mbufs[32];
      size_t nb_deq = ring_rx[pid][0]->deq_burst((void**)mbufs, 32);
      if (nb_deq == 0) continue;
      for (size_t i=0; i<nb_deq; i++) {

        size_t n = 0;
        for (size_t j=0; j<DELAY; j++) n++;

        int ret = ring_tx[pid^1][0]->enq(mbufs[i]);
        if (ret < 0) {
          rte_pktmbuf_free(mbufs[i]);
        }
      }
    }
  }
}

void wk1(void*)
{
  size_t nb_ports = ssn_dev_count();
  printf("start %s nb_ports=%zd lcore%zd\n", __func__, nb_ports, ssn_lcore_id());
  while (running) {
    for (size_t pid=0; pid<nb_ports; pid++) {
      rte_mbuf* mbufs[32];
      size_t nb_deq = ring_rx[pid][1]->deq_burst((void**)mbufs, 32);
      if (nb_deq == 0) continue;
      for (size_t i=0; i<nb_deq; i++) {

        size_t n = 0;
        for (size_t j=0; j<DELAY; j++) n++;

        int ret = ring_tx[pid^1][1]->enq(mbufs[i]);
        if (ret < 0) {
          rte_pktmbuf_free(mbufs[i]);
        }
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

  ring_rx[0][0] = new ssn_ring("rx0,0");
  ring_rx[0][1] = new ssn_ring("rx0,1");
  ring_rx[1][0] = new ssn_ring("rx1,0");
  ring_rx[1][1] = new ssn_ring("rx1,1");
  ring_tx[0][0] = new ssn_ring("tx0,0");
  ring_tx[0][1] = new ssn_ring("tx0,1");
  ring_tx[1][0] = new ssn_ring("tx1,0");
  ring_tx[1][1] = new ssn_ring("tx1,1");

  ssn_port_conf conf;
  size_t nb_ports = ssn_dev_count();
  for (size_t i=0; i<nb_ports; i++) {
    ssn_port_configure(i, &conf);
    ssn_port_dev_up(i);
    ssn_port_link_up(i);
    ssn_port_promisc_on(i);
  }

  ssn_native_thread_launch(l2fwd, nullptr, 2);
  ssn_native_thread_launch(wk0  , nullptr, 3);
  ssn_native_thread_launch(wk1  , nullptr, 4);
  wait_enter("PushEnterToExitL2fwd");
  running = false;
  ssn_sleep(1000);

  ssn_fin();
}


