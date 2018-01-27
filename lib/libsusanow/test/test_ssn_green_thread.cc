
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
 * @file   example_ssn_green_thread.cc
 * @brief  ssn_green_thread module example
 * @author Hiroki SHIROKURA
 * @date   2017.9.20
 */

#include <stdio.h>
#include <unistd.h>
#include <ssn_cpu.h>
#include <ssn_log.h>
#include <ssn_common.h>
#include <ssn_green_thread.h>
#include <dpdk/dpdk.h>

size_t num1=1;
size_t num2=2;

void test(void* arg)
{
  size_t* n = (size_t*)arg;
  for (size_t i=0; i<5; i++) {
    printf("test arg=%zd lcore%zd\n", *n, dpdk::lcore_id());
    ssn_sleep(1000);
  }
}

bool view_running = true;
void view(void*)
{
  while (view_running) {
    printf("\n");
    ssn_green_thread_debug_dump(stdout);
    printf("\n");
    ssn_sleep(1000);
  }
}

int main(int argc, char** argv)
{
  constexpr size_t gt_lcore_id = 3;
  ssn_log_set_level(SSN_LOG_EMERG);
  ssn_init(argc, argv);
  ssn_green_thread_sched_register(gt_lcore_id);
  ssn_green_thread_sched_register(gt_lcore_id+1);

  uint32_t tid0 = ssn_green_thread_launch(view, nullptr, gt_lcore_id);
  uint32_t tid1 = ssn_green_thread_launch(test, &num1, gt_lcore_id);
  uint32_t tid2 = ssn_green_thread_launch(test, &num2, gt_lcore_id+1);

  getchar();
  ssn_green_thread_join(tid2);
  printf("joined tid=%x\n", tid2);

  getchar();
  ssn_green_thread_join(tid1);
  printf("joined tid=%x\n", tid1);

  getchar();
  view_running = false;
  ssn_green_thread_join(tid0);
  printf("joined tid=%x. fin...\n", tid0);

  ssn_green_thread_sched_unregister(gt_lcore_id);
  ssn_green_thread_sched_unregister(gt_lcore_id+1);
  ssn_fin();
}


