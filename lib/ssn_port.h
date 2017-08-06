
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <rte_ethdev.h>

struct rte_eth_conf;
struct rte_mbuf;

class ssn_port_conf {
 public:
  size_t nb_rxq;
  size_t nb_txq;
  size_t nb_rxd;
  size_t nb_txd;
  ssn_port_conf();
  rte_eth_conf raw;
  void debug_dump(FILE* fp) const;
};

size_t ssn_dev_count();
void ssn_port_link_up(size_t p);
void ssn_port_link_down(size_t p);
void ssn_port_promisc_on(size_t pid);
void ssn_port_promisc_off(size_t pid);
void ssn_port_dev_up(size_t pid);
void ssn_port_dev_down(size_t pid);
void ssn_port_configure(size_t pid, ssn_port_conf* conf);
void ssn_port_init();
void ssn_port_fin();
void ssn_mbuf_free_bulk(rte_mbuf** m_list, size_t npkts);
size_t ssn_port_rx_burst(size_t pid, size_t qid, rte_mbuf** mbufs, size_t nb_mbufs);
size_t ssn_port_tx_burst(size_t pid, size_t qid, rte_mbuf** mbufs, size_t nb_mbufs);

