
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <vector>
#include <string>

#include <slankdev/extra/dpdk.h>
#include <slankdev/exception.h>
#include <slankdev/util.h>

#include <ssn_sys.h>
#include <ssn_common.h>
#include <ssn_timer.h>
#include <ssn_native_thread.h>
#include <ssn_green_thread.h>
#include <ssn_port_stat.h>
#include <ssn_log.h>
#include <ssn_ring.h>
#include <ssn_port.h>

#include "vnf.h"


void rx(void*);
void tx(void*);
void wk_bulk(void*);
void wk_shot(void*);


/*
 *---------------------------------------------------
 */

class ssn_vty;
class ssn_rest;

class ssn {
  const uint16_t vty_port = 9999;
  const uint16_t rest_port = 8888;
  const size_t required_nb_cores = 4;
  ssn_timer_sched* timer_sched;
  ssn_vty* vty;
  ssn_rest* rest;
  vnf* vnf0;
  std::vector<ssn_ring*> rings;
 public:
  ssn(int argc, char** argv);
  virtual ~ssn();
  ssn_ring* ring_alloc(const char* name);
  void add_timer(ssn_timer* tim) { timer_sched->add(tim); }
  void debug_dump(FILE* fp);
};




