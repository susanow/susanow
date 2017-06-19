
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <lthread.h>

#include <vector>
#include <queue>

#include <ssn_types.h>
#include <ssn_sys.h>



struct lthread_info {
  lthread* lt;
  ssn_function_t f;
  void* arg;
  bool dead;
};

struct launch_info {
  ssn_function_t f;
  void*        arg;
};

class ssn_lthread_sched {
 public:
  std::queue<launch_info> launch_queue;
  bool running;
  std::vector<lthread_info> threads;

  ssn_lthread_sched();
  void launch(ssn_function_t f, void* a);
  void start_scheduler(uint32_t lcore_id);
  void stop_scheduler();
  void debug_dump(FILE* fp);
};

extern ssn_sys sys;
inline void ssn_ltsched_register(size_t lcore_id)
{ sys.cpu.lcores[lcore_id].lthread_sched_register(); }
inline void ssn_ltsched_unregister(size_t lcore_id)
{ sys.cpu.lcores[lcore_id].lthread_sched_unregister(); }


