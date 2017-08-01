

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
#include <ssn_sys.h>
#include <ssn_native_thread.h>

class ssn_ring;
using ssn_rings = std::vector<ssn_ring*>;

class func {
 public:
  virtual void poll_exe() = 0;
  virtual void stop() = 0;
};

static inline ssize_t get_free_lcore_id()
{
  size_t nb_lcores = ssn_lcore_count();
  for (size_t i=1; i<nb_lcores; i++) {
    auto s = ssn_get_lcore_state(i);
    if (s == SSN_LS_WAIT) return i;
  }
  throw slankdev::exception("no lcore");
}

static inline void _func_spawner(void* arg)
{
  func* f = reinterpret_cast<func*>(arg);
  f->poll_exe();
}

class stage {
  std::vector<func*> funcs;
 public:
  const std::string name;

 public:
  stage(const char* n) : name(n) {}
  size_t mux() const { return funcs.size(); }
  void inc()
  {
    ssize_t lcore_id = get_free_lcore_id();
    func* f = falloc();
    funcs.push_back(f);
    ssn_native_thread_launch(_func_spawner, f, lcore_id);
  }
  void dec()
  {
    size_t idx = funcs.size()-1;
    funcs[idx]->stop();
    funcs.erase(funcs.begin() + idx);
  }
  virtual func*  falloc() = 0;
  virtual size_t throughput_pps() const = 0;
  virtual double reduction_rate() const = 0;
};


