
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
 * @file   example_ssn_vty.cc
 * @brief  ssn_vty module example
 * @author Hiroki SHIROKURA
 * @date   2017.9.20
 */
#include <ssn_vty.h>
#include <ssn_green_thread.h>
#include <ssn_native_thread.h>
#include <ssn_cpu.h>
#include <ssn_common.h>
#include <slankdev/util.h>
#include <slankdev/socketfd.h>

/* slank */
vty_cmd_match slank_mt()
{
  vty_cmd_match m;
  m.nodes.push_back(new node_fixedstring("slank", ""));
  return m;
}
void slank_f(vty_cmd_match* m, vty_client* sh, void* arg)
{
  size_t* n = reinterpret_cast<size_t*>(arg);
  sh->Printf("slankdev called %zd times\r\n", (*n)++);
}

int main(int argc, char** argv)
{
  size_t n = 0;
  ssn_init(argc, argv);

  ssn_vty vty(INADDR_ANY, 8888);
  vty.install_command(slank_mt(), slank_f, &n);

  ssn_green_thread_sched_register(1);
  ssn_green_thread_launch(ssn_vty_poll_thread, &vty, 1);
  getchar();
  ssn_vty_poll_thread_stop();
  ssn_green_thread_sched_unregister(1);

  ssn_wait_all_lcore();
  ssn_fin();
}
