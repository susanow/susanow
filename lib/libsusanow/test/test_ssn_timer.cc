
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
 * @file   example_ssn_timer.cc
 * @brief  ssn_timer module example
 * @author Hiroki SHIROKURA
 * @date   2017.9.20
 */

#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <ssn_timer.h>
#include <ssn_green_thread.h>
#include <ssn_native_thread.h>
#include <ssn_cpu.h>
#include <ssn_common.h>
#include <dpdk/dpdk.h>

size_t num0=0;
size_t num1=1;
size_t num2=2;
size_t num3=3;
size_t num4=4;
size_t num5=5;

void Slankdev (void* arg)
{
  int* n = (int*)arg;
  printf("slankdev arg=%d lcore%zd \n", *n, dpdk::lcore_id());
}

int main(int argc, char** argv)
{
  ssn_init(argc, argv);

  ssn_timer_sched tm2(2);
  ssn_timer_sched tm3(3);
  ssn_native_thread_launch(ssn_timer_sched_poll_thread, &tm2, 2);
  ssn_native_thread_launch(ssn_timer_sched_poll_thread, &tm3, 3);

  uint64_t hz = ssn_timer_get_hz();
  ssn_timer* tim2 = new ssn_timer(Slankdev , &num2, hz);
  ssn_timer* tim3 = new ssn_timer(Slankdev , &num3, hz);
  tm2.add(tim2);
  tm3.add(tim3);

  sleep(2);
  sleep(1); tm2.del(tim2); delete (tim2);
  sleep(2); tm3.del(tim3); delete (tim3);

  ssn_fin();
}


