

#pragma once

#include <vector>
#include <string>
#include <stdint.h>
#include <stddef.h>
#include <vector>
#include <string>
#include <stdint.h>
#include <stddef.h>
#include <slankdev/extra/dpdk.h>
#include <ssn_ring.h>
#include <ssn_port.h>
#include <ssn_port_stat.h>


class ssn_ring;
class func;
typedef func* (*allocate_func)(void*);


class stageio_rx final{
  ssn_ring* ring;
 public:
  void set(ssn_ring* r) { ring = r; }
  size_t rx_burst(rte_mbuf** obj_table, size_t n)
  { return ring->deq_bulk((void**)obj_table, n); }
  size_t rx_pps() const { return ring->opps; }
};
class stageio_tx final{
  ssn_ring* ring;
 public:
  void set(ssn_ring* r) { ring = r; }
  int tx_shot(rte_mbuf* obj) { return ring->enq((void*)obj); }
  size_t tx_burst(rte_mbuf** obj_table, size_t n)
  { return ring->enq_bulk((void* const*)obj_table, n); }
};

class vnic;
class stage final {
  size_t mux_;
 public:
  const std::string name;
  std::vector<func*> funcs;
  std::vector<stageio_rx*> rx;
  std::vector<stageio_tx*> tx;
  allocate_func     alloc_fn;

  stage(const char* n, allocate_func f)
    : name(n), mux_(0) , alloc_fn(f) {}

 public:
  void add_input_ring(ssn_ring* ring_ptr);
  void add_output_ring(ssn_ring* r);

 public:
  void add_nic(vnic* nic);

 public:
  size_t throughput_pps() const;
  size_t mux() const;
  void   inc();
  void   dec();
};


