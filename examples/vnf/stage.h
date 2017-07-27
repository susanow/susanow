

#pragma once

#include <vector>
#include <string>
#include <stdint.h>
#include <stddef.h>
#include "stageio.h"
#include <ssn_port.h>

static inline void _func_spawner(void* arg);
static inline ssize_t get_free_lcore_id();

class stageio_rx;
class stageio_tx;

class func {
 protected:
  bool run;
 public:
  std::vector<stageio_rx*> rx;
  std::vector<stageio_tx*> tx;
  virtual void poll_exe() = 0;
  virtual void stop() = 0;
};


typedef func* (*allocate_func)(void);

class stage final {
 protected:
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
  void add_input_port(size_t p)
  {
  }
  void add_input_ring(size_t n, )
  void in_set_ring(size_t n, ssn_ring* r)
  {
  }
  void out_resize(size_t n) { tx.resize(n); }
  void out_set_port(size_t n, size_t p)
  {
  }
  void out_set_ring(size_t n, ssn_ring* r)
  {
  }

 public:
  void inc()
  {
    func* f = alloc_fn();
    funcs.push_back(f);
    ssize_t lcore_id = get_free_lcore_id();
    if (lcore_id < 0) throw slankdev::exception("no lcore");
    ssn_native_thread_launch(_func_spawner, f, lcore_id);
    mux_ ++ ;
  }
  void dec()
  {
    size_t idx = funcs.size()-1;
    funcs[idx]->stop();
    funcs.erase(funcs.begin() + idx);
    mux_ -- ;
  }
  size_t mux() const { return mux_; }
  size_t throughput_pps() const
  {
    size_t sum_pps = 0;
    size_t nb_ports = rx.size();
    for (size_t i=0; i<nb_ports; i++) {
      sum_pps += rx[i].rx_pps();
    }
    return sum_pps;
  }
};


static inline void _func_spawner(void* arg)
{
  func* f = reinterpret_cast<func*>(arg);
  f->poll_exe();
}

static inline ssize_t get_free_lcore_id()
{
  size_t nb_lcores = ssn_lcore_count();
  for (size_t i=1; i<nb_lcores; i++) {
    auto s = ssn_get_lcore_state(i);
    if (s == SSN_LS_WAIT) return i;
  }
  return -1;
}
