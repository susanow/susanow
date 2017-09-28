
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
 * @file   example_ssn_native_thread.cc
 * @brief  ssn_native_thread module example
 * @author Hiroki SHIROKURA
 * @date   2017.9.20
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ssn_cpu.h>
#include <ssn_log.h>
#include <ssn_common.h>
#include <ssn_native_thread.h>

size_t num1 = 1;
size_t num2 = 2;

void test(void* arg)
{
  size_t* n = (size_t*)arg;
  for (size_t i=0; i<5; i++) {
    printf("test arg=%zd\n", *n);
    ssn_sleep(1000);
  }
}

bool view_running = true;
void view(void*)
{
  while (view_running) {
    ssn_native_thread_debug_dump(stdout);
    ssn_sleep(1000);
  }
}

int main(int argc, char** argv)
{
  ssn_log_set_level(SSN_LOG_EMERG);
  ssn_init(argc, argv);

  uint32_t tid0 = ssn_native_thread_launch(view, nullptr, 1);
  ssn_sleep(2000);
  uint32_t tid1 = ssn_native_thread_launch(test, &num1, 2);
  ssn_sleep(3000);
  uint32_t tid2 = ssn_native_thread_launch(test, &num2, 3);

  getchar();
  printf("before join%u\n", tid1);
  ssn_native_thread_join(tid1);
  printf("after join%u\n", tid1);

  getchar();
  printf("before join%u\n", tid2);
  ssn_native_thread_join(tid2);
  printf("after join%u\n", tid2);

  getchar();
  view_running = false;
  ssn_native_thread_join(tid0);
  printf("tid%u joined. all joined\n", tid0);

  ssn_fin();
}


