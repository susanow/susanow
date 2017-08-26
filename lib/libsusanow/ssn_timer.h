
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <ssn_types.h>
#include <rte_timer.h>

class ssn_timer {
 public:
  rte_timer tim;
  ssn_function_t f;
  void* arg;
  size_t hz;
  ssn_timer(ssn_function_t _f, void* _arg, size_t _hz);
  virtual ~ssn_timer();
};

class ssn_timer_sched {
 private:
  std::vector<ssn_timer*> tims;
  size_t lcore_id_;
 public:
  ssn_timer_sched(size_t i);
  virtual ~ssn_timer_sched();
  void add(ssn_timer* tim);
  void del(ssn_timer* tim);
  void debug_dump(FILE* fp);
  size_t lcore_id() const;
};
void ssn_timer_sched_poll_thread(void* arg);
void ssn_timer_sched_poll_thread_stop();

void ssn_timer_init();
void ssn_timer_fin();
uint64_t ssn_timer_get_hz();


