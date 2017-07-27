

#pragma once

#include <vector>
#include <string>
#include <stdint.h>
#include <stddef.h>

class stageio_rx;
class stageio_tx;
class ssn_ring;
class func;
typedef func* (*allocate_func)(void*);

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

