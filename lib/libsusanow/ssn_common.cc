
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

#include <unistd.h>
#include <lthread.h>
#include <dpdk/dpdk.h>
#include <ssn_cpu.h>
#include <ssn_common.h>
#include <ssn_types.h>
#include <ssn_timer.h>
#include <ssn_vty.h>
#include <ssn_green_thread.h>
#include <ssn_native_thread.h>
#include <ssn_log.h>
#include <ssn_port_stat.h>

void ssn_init(int argc, char** argv)
{
  ssn_cpu_init(argc, argv);
  ssn_native_thread_init();
  ssn_green_thread_init();
  ssn_timer_init();
  ssn_port_stat_init();
}


void ssn_subproc_init(int argc, char** argv)
{
  argc += 3;
  char opt1[] = "--proc-type=secondary";
  char opt2[] = "--socket-mem=2048,2048";
  char opt3[] = "-w 0000:00:00.0";
  char* wrapped_argv[argc];
  wrapped_argv[0] = argv[0];
  wrapped_argv[1] = opt1;
  wrapped_argv[2] = opt2;
  wrapped_argv[3] = opt3;
  for (size_t i=4,j=1; i<argc; i++,j++) {
    wrapped_argv[i] = argv[j];
  }
  ssn_init(argc, wrapped_argv);
}


void ssn_fin()
{
  ssn_native_thread_fin();
  ssn_green_thread_fin();
  ssn_timer_fin();
  ssn_port_stat_fin();
}

void ssn_wait_all_lcore()
{
  ssn_log(SSN_LOG_INFO, "wait all lcore\n");
  rte_eal_mp_wait_lcore();
  ssn_log(SSN_LOG_INFO, "all lcore was joined\n");
}


