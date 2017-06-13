
#pragma once
#include <lthread.h>
#include <queue>
#include <slankdev/extra/dpdk.h>

typedef void(*ssn_function_t)(void*);

class ssn_lthread_sched {
  friend void launcher(void*);
  struct call_info {
    ssn_function_t f;
    void*        arg;
  };
 private:
  bool running;
  std::queue<call_info> funcs;
 public:
  ssn_lthread_sched();
  void launch(ssn_function_t f, void* a);
  void start_scheduler(uint32_t lcore_id);
  void stop_scheduler();
};

ssn_lthread_sched* current_sched();


