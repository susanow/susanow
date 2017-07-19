
#pragma once

#include <vector>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <ssn_types.h>
#include "vnf.h"

class ssn_ring;
class ssn_vty;
class ssn_rest;

class ssn {
  const uint16_t vty_port = 9999;
  const uint16_t rest_port = 8888;
  const size_t required_nb_cores = 4;
  ssn_timer_sched* timer_sched;
  ssn_vty* vty;
  ssn_rest* rest;
  std::vector<ssn_ring*> rings;
 public:
  vnf_l2fwd* vnf1;

  ssn(int argc, char** argv);
  virtual ~ssn();

  void debug_dump(FILE* fp);
  ssn_ring* ring_alloc(const char* name);
  void add_timer(ssn_timer* tim) { timer_sched->add(tim); }
  void launch_green_thread(ssn_function_t f, void* arg);
};




