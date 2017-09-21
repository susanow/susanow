
/*
 * MIT License
 *
 * Copyright (c) 2017 Hiroki SHIROKURA
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/**
 * @file   ssn_native_thread.cc
 * @brief  native thread management
 * @author Hiroki SHIROKURA
 * @date   2017.9.20
 */

#include <ssn_cpu.h>
#include <ssn_log.h>
#include <ssn_native_thread.h>
#include <dpdk/dpdk.h>
#include <slankdev/exception.h>
#include <mutex>

using auto_lock=std::lock_guard<std::mutex>;
std::mutex mutex_thread_launch;
std::mutex mutex_thread_join;

class ssn_native_thread {
 public:
  ssn_function_t f;
  void*          arg;
};
ssn_native_thread* snt[RTE_MAX_LCORE];

class ssn_thread_id {
 public:
  uint16_t lcore_id;
  uint16_t sub_id;
};
std::vector<ssn_thread_id> tids;

void ssn_native_thread_init()
{
  size_t nb_lcores = rte_lcore_count();
  for (size_t i=0; i<nb_lcores; i++) {
    snt[i] = new ssn_native_thread;
  }
}

void ssn_native_thread_fin()
{
  size_t nb_lcores = rte_lcore_count();
  for (size_t i=0; i<nb_lcores; i++) {
    delete snt[i];
  }
}

static int _fthread_launcher(void* arg)
{
  size_t lcore_id = rte_lcore_id();
  ssn_native_thread* snt = reinterpret_cast<ssn_native_thread*>(arg);
  ssn_set_lcore_state(SSN_LS_RUNNING_NATIVE, lcore_id);
  snt->f(snt->arg);
  ssn_set_lcore_state(SSN_LS_FINISHED, lcore_id);
  return 0;
}

uint32_t ssn_native_thread_launch(ssn_function_t f, void* arg, size_t lcore_id)
{
  auto_lock lg(mutex_thread_launch);
  ssn_log(SSN_LOG_DEBUG, "launch native thread to lcore%zd \n", lcore_id);
  size_t n_lcore = rte_lcore_count();
  if (n_lcore <= lcore_id) {
    throw slankdev::exception("too huge lcore_id?");
  }

  ssn_thread_id tid;
  tid.lcore_id = lcore_id;
  tid.sub_id   = 0;
  tids.push_back(tid);

  snt[lcore_id]->f   = f;
  snt[lcore_id]->arg = arg;
  dpdk::rte_eal_remote_launch(_fthread_launcher, snt[lcore_id], lcore_id);
  return lcore_id;
}

void ssn_native_thread_join(uint32_t tid)
{
  auto_lock lg(mutex_thread_join);
  int ret = rte_eal_wait_lcore(tid);
  UNUSED(ret);

  size_t n_threads = tids.size();
  for (size_t i=0; i<n_threads; i++) {
    if (tids[i].lcore_id == tid) {
      tids.erase(tids.begin() + i);
      ssn_set_lcore_state(SSN_LS_WAIT, tid);
      ssn_log(SSN_LOG_DEBUG, "join lcore%zd \n", tid);
      return ;
    }
  }
  throw slankdev::exception("OKASHIII");
}

bool ssn_native_thread_joinable(uint32_t tid)
{
  rte_lcore_state_t s = rte_eal_get_lcore_state(tid);
  return s==FINISHED;
}

void ssn_native_thread_debug_dump(FILE* fp)
{
  fprintf(fp, "\r\n");
  fprintf(fp, " %-10s   %-10s   %-10s\r\n", "thread_id", "joinable", "state");
  fprintf(fp, " ----------------------------------------------------\r\n");
  size_t n_threads = tids.size();
  for (size_t i=0; i<n_threads; i++) {
    uint32_t tid = tids[i].lcore_id;
    auto state = ssn_get_lcore_state(tid);
    fprintf(fp, " 0x%08x   %-10s   %-10s \r\n", tid,
        ssn_native_thread_joinable(tid)?"yes":"no",
        ssn_lcore_state2str(state));
  }
  fprintf(fp, "\r\n");
}

#if 0
bool ssn_lcore_join_poll_thread_running;
void ssn_lcore_join_poll_thread_stop()
{ ssn_lcore_join_poll_thread_running=false; }
void ssn_lcore_join_poll_thread(void*)
{
  ssn_lcore_join_poll_thread_running = true;
  size_t lcore_id = ssn_lcore_id();
  while (ssn_lcore_join_poll_thread_running) {
    size_t nb_lcores = ssn_lcore_count();
    for (size_t i=0; i<nb_lcores; i++) {
      if (ssn_lcore_joinable(i)) {
        ssn_lcore_join(i);
      }
    }
    if (is_green_thread(lcore_id)) ssn_yield();
  }
}
#endif
