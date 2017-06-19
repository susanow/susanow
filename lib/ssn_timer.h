
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <ssn_sys.h>

class ssn_timer {
 public:
  rte_timer tim;
  ssn_function_t f;
  void* arg;
  size_t hz;
  size_t lcore_id;
  ssn_timer(ssn_function_t _f, void* _arg, size_t _hz, size_t id)
    : f(_f), arg(_arg), hz(_hz), lcore_id(id) {}
  virtual ~ssn_timer() {}
};

void ssn_tmsched_register(size_t lcore_id);
void ssn_tmsched_unregister(size_t lcore_id);
ssn_timer* ssn_timer_alloc(ssn_function_t f, void* arg, size_t hz, size_t lid);
void ssn_timer_free(ssn_timer* st);
void ssn_timer_add(ssn_timer* tim);
void ssn_timer_del(ssn_timer* st);
void ssn_timer_debug_dump(FILE* fp);



