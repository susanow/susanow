
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


