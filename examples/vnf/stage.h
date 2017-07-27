

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


class stageio_rx {
 public:
  virtual ~stageio_rx() {}
  virtual size_t rx_burst(rte_mbuf** obj_table, size_t n) = 0;
  virtual size_t rx_pps() const = 0;
};
class stageio_tx {
 public:
  virtual ~stageio_tx() {}
  virtual int tx_shot(rte_mbuf* obj) = 0;
  virtual size_t tx_burst(rte_mbuf** obj_table, size_t n) = 0;
};
class stageio_rx_ring final : public stageio_rx {
  ssn_ring* ring;
 public:
  void set(ssn_ring* r) { ring = r; }
  virtual size_t rx_burst(rte_mbuf** obj_table, size_t n) override;
  virtual size_t rx_pps() const override;
};
class stageio_tx_ring final : public stageio_tx {
  ssn_ring* ring;
 public:
  void set(ssn_ring* r) { ring = r; }
  virtual int tx_shot(rte_mbuf* obj) override;
  virtual size_t tx_burst(rte_mbuf** obj_table, size_t n) override;
};
class stageio_rx_port final : public stageio_rx {
  size_t pid;
 public:
  void set(size_t p);
  virtual size_t rx_burst(rte_mbuf** obj_table, size_t n) override;
  virtual size_t rx_pps() const override;
};
class stageio_tx_port final : public stageio_tx {
  size_t pid;
 public:
  void set(size_t p);
  virtual int tx_shot(rte_mbuf* obj) override;
  virtual size_t tx_burst(rte_mbuf** obj_table, size_t n) override;
};



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
  void add_input_port(size_t pid);
  void add_input_ring(ssn_ring* ring_ptr);
  void add_output_port(size_t pid);
  void add_output_ring(ssn_ring* r);

 public:
  size_t throughput_pps() const;
  size_t mux() const;
  void   inc();
  void   dec();
};


