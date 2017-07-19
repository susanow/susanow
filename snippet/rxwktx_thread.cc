

#include <slankdev/extra/dpdk.h>
#include <ssn_port.h>
#include <ssn_ring.h>

void rx(void*)
{
  size_t nb_ports = ssn_dev_count();
  while (run) {
    for (size_t p=0; p<nb_ports; p++) {
      rte_mbuf* mbufs[32];
      size_t recvlen = ssn_port_rx_burst(p, 0, mbufs, 32);
      if (recvlen == 0) continue;
      size_t enqlen = prewk[p]->enq_bulk((void**)mbufs, recvlen);
      if (recvlen > enqlen) {
        slankdev::rte_pktmbuf_free_bulk(&mbufs[enqlen], recvlen-enqlen);
      }
    }
  }
}

void tx(void*)
{
  size_t nb_ports = ssn_dev_count();
  while (run) {
    for (size_t p=0; p<nb_ports; p++) {
      rte_mbuf* mbufs[32];
      size_t deqlen = poswk[p]->deq_bulk((void**)mbufs, 32);
      if (deqlen == 0) continue;
      size_t sendlen = ssn_port_tx_burst(p, 0, mbufs, deqlen);
      if (deqlen > sendlen) {
        slankdev::rte_pktmbuf_free_bulk(&mbufs[sendlen], deqlen-sendlen);
      }
    }
  }
}

void wk_shot(void*)
{
  size_t nb_ports = ssn_dev_count();
  rte_mbuf* mbufs[32];
  while (run) {
    for (size_t p=0; p<nb_ports; p++) {
      size_t deqlen = prewk[p]->deq_bulk((void**)mbufs, 32);
      for (size_t i=0; i<deqlen; i++) {
        int ret = poswk[p^1]->enq(mbufs[i]);
        if (ret < 0) rte_pktmbuf_free(mbufs[i]);
      }
    }
  }
}
