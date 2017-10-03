
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
#include "vnf.h"


int main(int argc, char** argv)
{
  constexpr size_t n_ports_want = 2;
  ssn_init(argc, argv);
  size_t n_ports = ssn_dev_count();
  if (n_ports != n_ports_want) {
    std::string err = slankdev::format("n_ports is not %zd (current %zd)",
        n_ports_want, ssn_dev_count());
    throw slankdev::exception(err.c_str());
  }

  /*-------------------------------------------------------------------------*/

  ssn_vnf_port_dpdk* dpdk0 = new ssn_vnf_port_dpdk(0, 4, 4); // dpdk0
  ssn_vnf_port_dpdk* dpdk1 = new ssn_vnf_port_dpdk(1, 4, 4); // dpdk1
  ssn_vnf_port_virt* virt0 = new ssn_vnf_port_virt(4, 4); // virt
  ssn_vnf_port_virt* virt1 = new ssn_vnf_port_virt(4, 4); // virt

  dpdk0->debug_dump(stdout); printf("\n");
  dpdk1->debug_dump(stdout); printf("\n");
  virt0->debug_dump(stdout); printf("\n");
  virt1->debug_dump(stdout); printf("\n");

  /*-------------------------------------------------------------------------*/

  vnf v0("vnf0");
  v0.attach_port(0, dpdk0);
  v0.attach_port(1, virt0);

  vnf v1("vnf1");
  v1.attach_port(0, virt1);
  v1.attach_port(1, dpdk1);

  ssn_vnf_port_patch_panel pp(virt0, virt1);

  /*-------------------------------------------------------------------------*/

#if 1
  dpdk0->reset_acc();
  virt1->reset_acc();
  v0.set_coremask(0, 0x02); /* 0b00000010:0x02 */
  v0.config_port_acc();
  v0.deploy();

  virt1->reset_acc();
  dpdk1->reset_acc();
  v1.set_coremask(0, 0x04); /* 0b00000100:0x04 */
  v1.config_port_acc();
  v1.deploy();

  /*-------------------------------------------------------------------------*/

  getchar();
  v0.undeploy();
  v1.undeploy();
#endif

fin:
  delete dpdk0;
  delete dpdk1;
  delete virt0;
  delete virt1;
  ssn_fin();
}


