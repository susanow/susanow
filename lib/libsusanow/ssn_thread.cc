
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
 * @file   ssn_thread.cc
 * @brief  thread management
 * @author Hiroki SHIROKURA
 * @date   2017.9.20
 */

#include <ssn_log.h>
#include <ssn_thread.h>
#include <ssn_common.h>
#include <ssn_cpu.h>

#include <dpdk/dpdk.h>
#include <slankdev/exception.h>

bool ssn_tid_is_green_thread(uint32_t tid) { return ((tid&0xffff0000) != 0); }


uint32_t ssn_thread_launch(ssn_function_t f, void* arg, size_t lcore_id)
{
  if (lcore_id == 0)
    throw slankdev::exception("ssn_thread_launch: lcore_id is master's id");

  uint32_t (*thread_launcher)(ssn_function_t, void*, size_t) = nullptr;
  if (ssn_lcoreid_is_green_thread(lcore_id)) {
    /*
     * Green Thread
     */
    thread_launcher = ssn_green_thread_launch;
  } else {
    /*
     * Native Thread
     */
    thread_launcher = ssn_native_thread_launch;
  }
  return thread_launcher(f, arg, lcore_id);
}

void ssn_thread_join(uint32_t tid)
{
  if (ssn_tid_is_green_thread(tid)) {
    /*
     * Green Thread
     */
    ssn_green_thread_join(tid);
  } else {
    /*
     * Native Thread
     */
    ssn_native_thread_join(tid);
  }
}

bool ssn_thread_joinable(uint32_t tid)
{
  if (ssn_tid_is_green_thread(tid)) {
    /*
     * Green Thread
     */
    return ssn_green_thread_joinable(tid);
  } else {
    /*
     * Native Thread
     */
    return ssn_native_thread_joinable(tid);
  }
}

void ssn_thread_debug_dump(FILE* fp)
{
  ssn_native_thread_debug_dump(fp);
  ssn_green_thread_debug_dump(fp);
}


