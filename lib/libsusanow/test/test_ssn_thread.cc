
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
 * @file   example_ssn_thread.cc
 * @brief  ssn_thread module example
 * @author Hiroki SHIROKURA
 * @date   2017.9.20
 */

#include <stdio.h>
#include <ssn_cpu.h>
#include <ssn_log.h>
#include <ssn_common.h>
#include <ssn_thread.h>

size_t num0 = 0;
size_t num1 = 1;
size_t num2 = 2;

void func(void* arg)
{
  auto lcore_id = ssn_lcore_id();
  size_t num = *reinterpret_cast<size_t*>(arg);
  for (size_t i=0; i<3; i++) {
    printf("%02lx: func arg=%zd lcore%zd\n", i, num, lcore_id);
    ssn_sleep(1000);
  }
}

int main(int argc, char** argv)
{
  ssn_log_set_level(SSN_LOG_EMERG);
  ssn_init(argc, argv);
  ssn_green_thread_sched_register(1);

  auto tid = ssn_thread_launch(func, &num1, 1);
  ssn_thread_join(tid);

  ssn_green_thread_sched_unregister(1);
  ssn_wait_all_lcore();
  ssn_fin();
}




