
#pragma once
#include <vector>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <ssn_types.h>


class ssn_sys;
extern ssn_sys sys;
void ssn_waiter_thread(void*);


class ssn_lcore {
  friend int _fthread_launcher(void*);
 private:
  size_t id;
  ssn_function_t f;
  void*          arg;
 public:
  ssn_lcore_state state;

  ssn_lcore() : id(0), state(SSN_LS_WAIT) {}
  ~ssn_lcore() {}

  void init(size_t i, ssn_lcore_state s);
  void debug_dump(FILE* fp) const;
  void launch(ssn_function_t _f, void* _arg);
  void wait();
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

ssn_sys* ssn_get_sys();
void ssn_launch(ssn_function_t f, void* arg, size_t lcore_id);
void ssn_sys_init(int argc, char** argv);
void ssn_wait(size_t lcore_id);
bool ssn_cpu_debug_dump(FILE* fp);
ssn_lcore_state ssn_get_lcore_state(size_t lcore_id);

