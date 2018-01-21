
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
#include <ssn_vnf.h>
#include <ssn_vnf_port_dpdk.h>
#include <ssn_vnfs/l2fwd1b_delay.h>

int main(int argc, char** argv)
{
  ssn_init(argc, argv);
  size_t n_ports = ssn_dev_count();
  // if (n_ports != 2) {
  //   std::string err = slankdev::format("n_ports is not 2 (current %zd)",
  //       ssn_dev_count());
  //   throw slankdev::exception(err.c_str());
  // }
  printf("n_port:%zd\n", n_ports);

  //-------------------------------------------------------

  rte_mempool* mp0 = dpdk::mp_alloc("ssn0", 0, 81920);
  rte_mempool* mp1 = dpdk::mp_alloc("ssn1", 1, 81920);
  ssn_vnf_port_dpdk dpdk0("dpdk0", 0);
  ssn_vnf_port_dpdk dpdk1("dpdk1", 1);
  ssn_vnf_port_dpdk dpdk2("dpdk2", 2);
  ssn_vnf_port_dpdk dpdk3("dpdk3", 3);

  dpdk0.set_mp(mp0);
  dpdk1.set_mp(mp0);
  dpdk2.set_mp(mp1);
  dpdk3.set_mp(mp1);

  dpdk0.config_hw(6,6);
  dpdk1.config_hw(6,6);
  dpdk2.config_hw(6,6);
  dpdk3.config_hw(6,6);

  //-------------------------------------------------------

  ssn_vnf_l2fwd1b_delay v0("vnf0");
  v0.attach_port(0, &dpdk0);
  v0.attach_port(1, &dpdk1);

  ssn_vnf_l2fwd1b_delay v1("vnf0");
  v1.attach_port(0, &dpdk2);
  v1.attach_port(1, &dpdk3);

  //-------------------------------------------------------

  v0.reset();
  v1.reset();

  v0.set_coremask(0, 0b01111110);
  v1.set_coremask(0, 0b01111110000000);

  v0.deploy();
  v1.deploy();

  getchar();
  v0.undeploy();
  v1.undeploy();

  //-------------------------------------------------------

  // v0.reset();
  // v0.set_coremask(0, 0b00000110);
  // v0.deploy();
  // getchar();
  // v0.undeploy();
  //
  // //-------------------------------------------------------
  //
  // v0.reset();
  // v0.set_coremask(0, 0b00011110);
  // v0.deploy();
  // getchar();
  // v0.undeploy();

  //-------------------------------------------------------

  rte_mempool_free(mp0);
  rte_mempool_free(mp1);
  ssn_fin();
}


