
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



class ssn_sys;
extern ssn_sys sys;
typedef void(*ssn_function_t)(void*);
void ssn_waiter_thread(void*);

enum ssn_lcore_state {
  SSN_LS_WAIT,
  SSN_LS_RUNNING_LTHREAD,
  SSN_LS_RUNNING_NATIVE,
  SSN_LS_RUNNING_TIMER,
  SSN_LS_FINISHED,
};
inline const char* ssn_lcore_state2str(enum ssn_lcore_state e)
{
  switch (e) {
    case SSN_LS_WAIT           : return "WAIT"    ;
    case SSN_LS_RUNNING_LTHREAD: return "RUNLTH"  ;
    case SSN_LS_RUNNING_NATIVE : return "RUNNAT"  ;
    case SSN_LS_RUNNING_TIMER  : return "RUNTMR"  ;
    case SSN_LS_FINISHED       : return "FINISHED";
    default: return "UNKNOWN";
  }
}

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
  friend void launcher(void*);
 private:
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


inline void ssn_init(int argc, char** argv)
{ sys.init(argc, argv); }
inline void ssn_launch(ssn_function_t f, void* arg, size_t lcore_id)
{ sys.cpu.lcores[lcore_id].launch(f, arg); }
inline void ssn_wait(size_t lcore_id)
{ sys.cpu.lcores[lcore_id].wait(); }
inline ssn_sys* ssn_get_sys()
{ return &sys; }
inline void ssn_ltsched_register(size_t lcore_id)
{ sys.cpu.lcores[lcore_id].lthread_sched_register(); }
inline void ssn_ltsched_unregister(size_t lcore_id)
{ sys.cpu.lcores[lcore_id].lthread_sched_unregister(); }


inline ssn_lcore_state ssn_get_lcore_state(size_t lcore_id)
{ return sys.cpu.lcores[lcore_id].state; }
inline bool is_lthread(size_t lcore_id)
{ return ssn_get_lcore_state(lcore_id) == SSN_LS_RUNNING_LTHREAD; }
inline bool is_tthread(size_t lcore_id)
{ return ssn_get_lcore_state(lcore_id) == SSN_LS_RUNNING_TIMER; }
inline bool ssn_cpu_debug_dump(FILE* fp)
{ sys.cpu.debug_dump(fp); }

inline void ssn_sleep(size_t msec)
{
  if (is_lthread(rte_lcore_id())) lthread_sleep(msec);
  else usleep(msec * 1000);
}

