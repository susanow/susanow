
/*
 * MIT License
 *
 * Copyright (c) 2017 Susanow
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

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <slankdev/string.h>
#include <slankdev/exception.h>

#include <ssn_port.h>
#include <ssn_common.h>
#include <ssn_log.h>
#include <dpdk/dpdk.h>
#include <ssn_vnf_v02_l2fwd1b.h>
#include <ssn_port_stat.h>


bool running;
void print_perf(void* arg)
{
  ssn_vnf_l2fwd1b* v0 = reinterpret_cast<ssn_vnf_l2fwd1b*>(arg);

  running = true;
  while (running) {
    v0->debug_dump(stdout);
    ssn_sleep(1000);
    ssn_yield();
  }
}

void update_stats(void* arg)
{
  ssn_vnf_l2fwd1b* v0 = reinterpret_cast<ssn_vnf_l2fwd1b*>(arg);

  while (running) {
    v0->update_stats();
    ssn_port_stat_update(nullptr);
    ssn_sleep(1000);
    ssn_yield();
  }
}

int main(int argc, char** argv)
{
  ssn_init(argc, argv);
  size_t n_ports = ssn_dev_count();
  if (n_ports != 2) {
    std::string err = slankdev::format("n_ports is not 2 (current %zd)",
        ssn_dev_count());
    throw slankdev::exception(err.c_str());
  }

  constexpr size_t green_thread_lid = 7;
  ssn_green_thread_sched_register(green_thread_lid);

  rte_mempool* mp = dpdk::mp_alloc("ssn");
  ssn_vnf_port* port0 = new ssn_vnf_port_dpdk("dpdk0", 0, 4, 4, mp); // dpdk0
  ssn_vnf_port* port1 = new ssn_vnf_port_dpdk("dpdk1", 1, 4, 4, mp); // dpdk1
  printf("\n");
  port0->debug_dump(stdout); printf("\n");
  port1->debug_dump(stdout); printf("\n");

  ssn_vnf_l2fwd1b v0("vnf0");
  v0.attach_port(0, port0);
  v0.attach_port(1, port1);
  uint64_t tid0 = ssn_thread_launch(print_perf, &v0, green_thread_lid);
  uint64_t tid1 = ssn_thread_launch(update_stats, &v0, green_thread_lid);

  //-------------------------------------------------------

  v0.reset_allport_acc();
  v0.set_coremask(0, 0b00000010);
  v0.deploy();
  getchar();
  v0.undeploy();

  //-------------------------------------------------------

  v0.reset_allport_acc();
  v0.set_coremask(0, 0b00000110);
  v0.deploy();
  getchar();
  v0.undeploy();

  //-------------------------------------------------------

  v0.reset_allport_acc();
  v0.set_coremask(0, 0b00011110);
  v0.deploy();
  getchar();
  v0.undeploy();

fin:
  running = false;
  ssn_thread_join(tid0);
  ssn_thread_join(tid1);
  rte_mempool_free(mp);
  delete port0;
  delete port1;
  ssn_fin();
}


