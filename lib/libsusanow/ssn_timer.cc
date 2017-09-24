

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

#include <assert.h>
#include <ssn_cpu.h>
#include <ssn_timer.h>
#include <ssn_native_thread.h>
#include <slankdev/exception.h>
#include <slankdev/string.h>
#include <dpdk/hdr.h>

static void _SSN_TIMER_LAUNCHER(struct rte_timer* tim, void* arg)
{
  ssn_timer* st = reinterpret_cast<ssn_timer*>(arg);
  st->f(st->arg);
}

/*
 * ssn_timer Member Function
 */
ssn_timer::ssn_timer(ssn_function_t _f, void* _arg, size_t _hz)
  : f(_f), arg(_arg), hz(_hz) { rte_timer_init(&tim); }
ssn_timer::~ssn_timer() {}

/*
 * ssn_timer_sched Member Function
 */
ssn_timer_sched::ssn_timer_sched(size_t i) : lcore_id_(i) {}
ssn_timer_sched::~ssn_timer_sched() {}
size_t ssn_timer_sched::lcore_id() const { return lcore_id_; }
void ssn_timer_sched::add(ssn_timer* tim)
{
  tims.push_back(tim);
  if (!ssn_lcoreid_is_tthread(lcore_id_)) {
    ssn_lcore_state s = ssn_get_lcore_state(lcore_id_);
    std::string e;
    e += "ssn_timer_add: ";
    e += slankdev::format("lcore%zd is not timer thread", lcore_id_);
    e += slankdev::format(" (current: %s)", ssn_lcore_state2str(s));
    throw slankdev::exception(e.c_str());
  }
  rte_timer_reset(&tim->tim, tim->hz, PERIODICAL, lcore_id_, _SSN_TIMER_LAUNCHER, tim);
}
void ssn_timer_sched::del(ssn_timer* tim)
{
  if (!ssn_lcoreid_is_tthread(lcore_id_)) {
    ssn_lcore_state s = ssn_get_lcore_state(lcore_id_);
    std::string e;
    e += "ssn_timer_del: ";
    e += slankdev::format("lcore%zd is not timer thread", lcore_id_);
    e += slankdev::format(" (current: %s)", ssn_lcore_state2str(s));
    throw slankdev::exception(e.c_str());
  }
  rte_timer_stop(&tim->tim);

  for (size_t i=0; i<tims.size(); i++) {
    if (tims[i] == tim) {
      tims.erase(tims.begin() + i);
      return ;
    }
  }
  throw slankdev::exception("not found timer object");
}
void ssn_timer_sched::debug_dump(FILE* fp)
{
  fprintf(fp, "ssn_timer\r\n");
  fprintf(fp, "%5s: %-15s %-15s %-15s \r\n", "idx", "rte_timer", "funcptr", "arg");
  fprintf(fp, "--------------------------------------");
  fprintf(fp, "-----------------------------------\r\n");
  for (size_t i=0; i<tims.size(); i++) {
    fprintf(fp, "[%3zd]: %-15p %-15p %-15p \r\n", i,
        &tims[i]->tim,
         tims[i]->f,
         tims[i]->arg);
  }
}

/*
 * Other Function
 */
void ssn_timer_init() { rte_timer_subsystem_init(); }
void ssn_timer_fin() {}
uint64_t ssn_timer_get_hz() { return rte_get_timer_hz(); }
bool ssn_timer_sched_poll_thread_running;
void ssn_timer_sched_poll_thread_stop() { ssn_timer_sched_poll_thread_running = false; }
void ssn_timer_sched_poll_thread(void* arg)
{
  ssn_timer_sched* mgr = reinterpret_cast<ssn_timer_sched*>(arg);
  size_t lcore_id = mgr->lcore_id();
  ssn_set_lcore_state(SSN_LS_RUNNING_TIMER, lcore_id);

  constexpr size_t TIMER_RESOLUTION_CYCLES = 20000000ULL; /* around 10ms at 2 Ghz */
  uint64_t cur_tsc;
  uint64_t diff_tsc;
  uint64_t prev_tsc;

  assert(lcore_id == rte_lcore_id());
  ssn_timer_sched_poll_thread_running = true;
  while (ssn_timer_sched_poll_thread_running) {
    cur_tsc = rte_rdtsc();
    diff_tsc = cur_tsc - prev_tsc;

    if (diff_tsc > TIMER_RESOLUTION_CYCLES) {
      rte_timer_manage();
      prev_tsc = cur_tsc;
    }
  }
  ssn_set_lcore_state(SSN_LS_RUNNING_NATIVE, lcore_id);
}


