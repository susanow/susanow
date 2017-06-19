
#pragma once
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <slankdev/exception.h>
#include <slankdev/extra/dpdk.h>
#include <slankdev/extra/dpdk_struct.h>
#include <lthread.h>
#include <slankdev/extra/dpdk.h>
#include <queue>

#include <ssn_types.h>

class ssn_sys;
extern ssn_sys sys;
class ssn_lthread_sched;
void ssn_waiter_thread(void*);


class ssn_lcore {
  friend int _fthread_launcher(void*);
 private:
  size_t id;
  ssn_function_t f;
  void*          arg;
 public:
  ssn_lcore_state state;
  ssn_lthread_sched* lt_sched;

  ssn_lcore() : id(0), state(SSN_LS_WAIT), lt_sched(nullptr) {}

  ~ssn_lcore() {}

  void init(size_t i, ssn_lcore_state s);
  void debug_dump(FILE* fp) const;
  void launch(ssn_function_t _f, void* _arg);
  void wait();

  void lthread_sched_register();
  void lthread_sched_unregister();
};

class ssn_cpu {
 public:
  std::vector<ssn_lcore> lcores;
  ssn_cpu() {}
  ~ssn_cpu() {}
  void init(size_t nb);
  void debug_dump(FILE* fp) const;
};

class ssn_sys {
 public:
  ssn_cpu cpu;
  ssn_sys() {}
  ~ssn_sys() {}
  void init(int argc, char** argv);
};




