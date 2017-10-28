
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
#include <dpdk/dpdk.h>

#include <ssn_port.h>
#include <ssn_common.h>
#include <ssn_log.h>
#include <ssn_vnf_v02_l2fwd1b.h>


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
  ssn_vnf_port_dpdk* dpdk0 = new ssn_vnf_port_dpdk(0, 4, 4, mp); // dpdk0
  ssn_vnf_port_dpdk* dpdk1 = new ssn_vnf_port_dpdk(1, 4, 4, mp); // dpdk1
  ssn_vnf_port_virt* virt0 = new ssn_vnf_port_virt(4, 4); // virt
  ssn_vnf_port_virt* virt1 = new ssn_vnf_port_virt(4, 4); // virt
  ssn_vnf_port_patch_panel pp(virt0, virt1, 8);

  /*-------------------------------------------------------------------------*/

  ssn_vnf_l2fwd1b v0("vnf0");
  v0.attach_port(0, dpdk0);
  v0.attach_port(1, virt0);
  v0.reset_allport_acc();
  v0.set_coremask(0, 0x02);
  v0.deploy();

  ssn_vnf_l2fwd1b v1("vnf1");
  v1.attach_port(0, virt1);
  v1.attach_port(1, dpdk1);
  v1.reset_allport_acc();
  v1.set_coremask(0, 0x04);
  v1.deploy();

  /*-------------------------------------------------------------------------*/

  getchar();
  v0.undeploy();
  v1.undeploy();

fin:
  rte_mempool_free(mp);
  delete dpdk0;
  delete dpdk1;
  delete virt0;
  delete virt1;
  ssn_fin();
}


