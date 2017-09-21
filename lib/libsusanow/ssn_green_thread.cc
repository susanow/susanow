

#include <stdint.h>
#include <ssn_log.h>
#include <ssn_cpu.h>
#include <ssn_types.h>
#include <ssn_green_thread.h>
#include <ssn_native_thread.h>

#include <stdio.h>
#include <stdlib.h>
#include <lthread.h>
#include <dlfcn.h>
#include <vector>
#include <mutex>
#include <queue>
#include <dpdk/hdr.h>
#include <slankdev/exception.h>

using auto_lock=std::lock_guard<std::mutex>;
std::mutex m;
static void _ssn_thread_spawner(void* arg);

class ssn_green_thread {
 public:
  lthread* lt;
  ssn_function_t f;
  void* arg;
  size_t lcore_id;
  bool dead;
  uint32_t tid;

  ssn_green_thread(ssn_function_t _f, void* _arg, size_t _lcore_id)
    : lt(nullptr), f(_f), arg(_arg), lcore_id(_lcore_id), dead(false)
  {
    static uint32_t subtid = 1;
    tid = lcore_id + (subtid << 16);
    subtid++;
    lthread_create(&lt, lcore_id, _ssn_thread_spawner, this);
  }
  virtual ~ssn_green_thread() {}
};
std::vector<ssn_green_thread*> threads;

static void _ssn_thread_spawner(void* arg)
{
  ssn_green_thread* sl = reinterpret_cast<ssn_green_thread*>(arg);
  sl->f(sl->arg);
  sl->dead = true;
}

bool _green_thread_dummy_running[RTE_MAX_LCORE];
static void _green_thread_dummy(void*)
{
  size_t lcore_id = rte_lcore_id();
  _green_thread_dummy_running[lcore_id] = true;
  while (_green_thread_dummy_running[lcore_id]) {
    ssn_sleep(1000);
    ssn_yield();
  }
}
static void _green_thread_master(void*)
{
  size_t lcore_id = rte_lcore_id();
  ssn_log(SSN_LOG_INFO, "_green_thread_master: call lthread_run() on lcore%zd\n", lcore_id);
  lthread* lt;
  lthread_create(&lt, lcore_id, _green_thread_dummy, nullptr);
  lthread_run();
  ssn_log(SSN_LOG_INFO, "_green_thread_master: ret lthread_run() on lcore%zd\n", lcore_id);
}

uint32_t ssn_green_thread_launch(ssn_function_t f, void* arg, size_t lcore_id)
{
  if (!is_green_thread(lcore_id))
    throw slankdev::exception("is not green thread lcore");

  auto_lock lg(m);
  ssn_green_thread* sl = new ssn_green_thread(f, arg, lcore_id);
  threads.push_back(sl);
  return sl->tid;
}

void ssn_green_thread_join(uint32_t tid)
{
  while (true) {
    if (ssn_green_thread_joinable(tid)) break;
  }

  auto_lock lg(m);
  size_t n_threads = threads.size();
  for (size_t i=0; i<n_threads; i++) {
    if (threads[i]->tid == tid) {
      ssn_green_thread* sgt = threads[i];
      threads.erase(threads.begin() + i);
      delete sgt;
      return ;
    }
  }
  throw slankdev::exception("ssn_green_thread_join: tid not found");
}

bool ssn_green_thread_joinable(uint32_t tid)
{
  auto_lock lg(m);
  size_t n_threads = threads.size();
  for (size_t i=0; i<n_threads; i++) {
    if (threads[i]->tid == tid) {
      return threads[i]->dead;
    }
  }
  throw slankdev::exception("ssn_green_thread_joinable: tid not found");
}

void ssn_green_thread_sched_register(size_t lcore_id)
{
  ssn_native_thread_launch(_green_thread_master, nullptr, lcore_id);
  ssn_set_lcore_state(SSN_LS_RUNNING_GREEN, lcore_id);
}

void ssn_green_thread_sched_unregister(size_t lcore_id)
{
  if (!is_green_thread(lcore_id))
    throw slankdev::exception("is not green thread lcore");

  _green_thread_dummy_running[lcore_id] = false;
  lthread_scheduler_force_shutdown(lcore_id);
  ssn_set_lcore_state(SSN_LS_FINISHED, lcore_id);
}

void ssn_green_thread_debug_dump(FILE* fp)
{
  fprintf(fp, "\r\n");
  fprintf(fp, " %-10s   %-10s   %-10s\r\n", "thread_id", "joinable", "state");
  fprintf(fp, " ----------------------------------------------------\r\n");
  size_t n_threads = threads.size();
  for (size_t i=0; i<n_threads; i++) {
    uint32_t tid = threads[i]->tid;
    fprintf(fp, " 0x%08x   %-10s   \r\n", tid,
        ssn_green_thread_joinable(tid)?"yes":"no");
  }
  fprintf(fp, "\r\n");
}

void ssn_green_thread_init() {}
void ssn_green_thread_fin() {}


