
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
#include <ssn_vnf_l2fwd1b.h>


int main(int argc, char** argv)
{
  ssn_init(argc, argv);
  size_t n_ports = ssn_dev_count();
  if (n_ports != 2) {
    std::string err = slankdev::format("n_ports is not 2 (current %zd)",
        ssn_dev_count());
    throw slankdev::exception(err.c_str());
  }

  /*-------------------------------------------------------------------------*/

  rte_mempool* mp = dpdk::mp_alloc("ssn");
  ssn_vnf_port_dpdk dpdk0("dpdk0", 0, mp);
  ssn_vnf_port_dpdk dpdk1("dpdk1", 1, mp);
  ssn_vnf_port_virt virt0("virt0");
  ssn_vnf_port_virt virt1("virt1");
  dpdk0.config_hw(4, 4);
  dpdk1.config_hw(4, 4);
  virt0.config_hw(4, 4);
  virt1.config_hw(4, 4);
  ssn_vnf_port_patch_panel pp("ppp0", &virt0, &virt1);

  /*-------------------------------------------------------------------------*/

  ssn_vnf_l2fwd1b vnf0("vnf0");
  vnf0.attach_port(0, &dpdk0);
  vnf0.attach_port(1, &virt0);

  ssn_vnf_l2fwd1b vnf1("vnf1");
  vnf1.attach_port(0, &virt1);
  vnf1.attach_port(1, &dpdk1);

  /*-------------------------------------------------------------------------*/

  vnf0.reset();
  vnf1.reset();
  vnf0.set_coremask(0, 0b00000010);
  vnf1.set_coremask(0, 0b00000100);
  vnf0.deploy();
  vnf1.deploy();

  getchar();
  vnf0.undeploy();
  vnf1.undeploy();

  vnf0.reset();
  vnf1.reset();
  vnf0.set_coremask(0, 0b00000110);
  vnf1.set_coremask(0, 0b00011000);
  vnf0.deploy();
  vnf1.deploy();

  getchar();
  vnf0.undeploy();
  vnf1.undeploy();

  /*-------------------------------------------------------------------------*/

  rte_mempool_free(mp);
  ssn_fin();
}


