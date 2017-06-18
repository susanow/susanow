
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


#if 0
template<class T>
class vector_ts {
    std::vector<T> vec;
    mutable std::mutex m;
    using auto_lock=std::lock_guard<std::mutex>;
public:
    void push_back(T msg)
    {
        auto_lock lg(m);
        vec.push_back(msg);
    }
    T pop_back()
    {
        auto_lock lg(m);
        T msg = vec.pop_back();
        return msg;
    }
    size_t size() const
    {
        auto_lock lg(m);
        return vec.size();
    }
    bool empty() const
    {
        auto_lock lg(m);
        return vec.empty();
    }
    T operator[](size_t i) const
    {
      auto_lock lg(m);
      return vec[i];
    }
    void Erase(size_t pos)
    {
      auto_lock lg(m);
      vec.erase(vec.begin() + pos);
    }
};
#endif


class ssn_sys;
extern ssn_sys sys;
typedef void(*ssn_function_t)(void*);

enum ssn_lcore_state {
  SSN_LS_WAIT,
  SSN_LS_RUNNING_LTHREAD,
  SSN_LS_RUNNING_NATIVE,
  SSN_LS_FINISHED,
};
inline const char* ssn_lcore_state2str(enum ssn_lcore_state e)
{
  switch (e) {
    case SSN_LS_WAIT           : return "WAIT    ";
    case SSN_LS_RUNNING_LTHREAD: return "RUN_LTH ";
    case SSN_LS_RUNNING_NATIVE : return "RUN_NAT ";
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

#if 0
class ssn_timer_sched {
  size_t lcore_id;
  std::vector<rte_timer*> tims;
 public:
  ssn_timer_sched(size_t lid) : lcore_id(lid) {}
  virtual ~ssn_timer_sched() {}
  void start_scheduler(uint32_t lcore_id);
  void stop_scheduler();

  void add(rte_timer* tim, ssn_function_t f, void* arg);
  void del(rte_timer* tim);

  void debug_dump(FILE* fp);
};
#endif


class ssn_lcore {
  friend int _fthread_launcher(void*);
 private:
  size_t id;
  ssn_function_t f;
  void*          arg;
 public:
  ssn_lcore_state state;
  ssn_lthread_sched* lt_sched;
#if 0
  ssn_timer_sched*   tm_sched;
#endif

  ssn_lcore() : id(0), state(SSN_LS_WAIT),
  lt_sched(nullptr)
#if 0
  tm_sched(nullptr)
#endif
  {}

  ~ssn_lcore() {}

  void init(size_t i, ssn_lcore_state s);
  void debug_dump(FILE* fp) const;
  void launch(ssn_function_t _f, void* _arg);
  void wait();

  void lthread_sched_register();
  void lthread_sched_unregister();

#if 0
  void timer_sched_register();
  void timer_sched_unregister();
  void timer_sched_add(rte_timer* tim, ssn_function_t f, void* arg);
  void timer_sched_del(rte_timer* tim);
#endif

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


inline bool is_lthread(size_t lcore_id)
{ return (sys.cpu.lcores[lcore_id].lt_sched); }
inline bool is_lthread()
{
  size_t lid = rte_lcore_id();
  return (sys.cpu.lcores[lid].lt_sched);
}
inline void ssn_sleep(size_t msec)
{
  if (is_lthread()) lthread_sleep(msec);
  else usleep(msec * 1000);
}
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

#if 0
inline void ssn_timersched_register(size_t lcore_id)
{ sys.cpu.lcores[lcore_id].timer_sched_register(); }
inline void ssn_timersched_unregister(size_t lcore_id)
{ sys.cpu.lcores[lcore_id].timer_sched_unregister(); }

inline void ssn_timer_add(rte_timer* tim, ssn_function_t f, void* arg, size_t lcore_id)
{ sys.cpu.lcores[lcore_id].timer_sched_add(tim, f, arg); }
inline void ssn_timer_del(rte_timer* tim, size_t lcore_id)
{ sys.cpu.lcores[lcore_id].timer_sched_del(tim); }
#endif

