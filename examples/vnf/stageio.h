
#pragma once

#include <vector>
#include <string>
#include <stdint.h>
#include <stddef.h>
#include <slankdev/extra/dpdk.h>
#include <ssn_ring.h>
#include <ssn_port.h>
#include <ssn_port_stat.h>


class stageio_rx {
 public:
  stageio_rx() {}
  virtual ~stageio_rx() {}
  virtual size_t rx_burst(rte_mbuf** obj_table, size_t n) = 0;
  virtual size_t rx_pps() const = 0;
};
class stageio_tx {
 public:
  stageio_tx() {}
  virtual ~stageio_tx() {}
  virtual int tx_shot(rte_mbuf* obj) = 0;
  virtual size_t tx_burst(rte_mbuf** obj_table, size_t n) = 0;
};

class stageio_rx_ring : public stageio_rx {
 public:
  ssn_ring* ring;

  void set(ssn_ring* r) { ring = r; }
  virtual size_t rx_burst(rte_mbuf** obj_table, size_t n) override
  { return ring->deq_bulk((void**)obj_table, n); }
  virtual size_t rx_pps() const override { return ring->opps; }
};
class stageio_tx_ring : public stageio_tx {
 public:
  ssn_ring* ring;

  void set(ssn_ring* r) { ring = r; }
  virtual int tx_shot(rte_mbuf* obj) override { return ring->enq((void*)obj); }
  virtual size_t tx_burst(rte_mbuf** obj_table, size_t n) override
  { return ring->enq_bulk((void* const*)obj_table, n); }
};

class stageio_rx_port : public stageio_rx {
 public:
  size_t pid;

  void set(size_t p) { pid = p; }
  virtual size_t rx_burst(rte_mbuf** obj_table, size_t n) override
  { return rte_eth_rx_burst(pid, 0, obj_table, n); }
  virtual size_t rx_pps() const override { return  ssn_port_stat_get_cur_rx_pps(pid); }
};
class stageio_tx_port : public stageio_tx {
 public:
  size_t pid;

  void set(size_t p) { pid = p; }
  virtual int tx_shot(rte_mbuf* obj) override
  { throw slankdev::exception("not imple"); }
  virtual size_t tx_burst(rte_mbuf** obj_table, size_t n) override
  { return rte_eth_tx_burst(pid, 0, obj_table, n); }
};


// using txs = std::vector<stageio_tx>;
// using rxs = std::vector<stageio_rx>;
using rxports = std::vector<stageio_rx_port>;
using txports = std::vector<stageio_tx_port>;
using rxrings = std::vector<stageio_rx_ring>;
using txrings = std::vector<stageio_tx_ring>;



