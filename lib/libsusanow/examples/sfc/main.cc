
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
  constexpr size_t n_ports_want = 4;
  ssn_init(argc, argv);
  size_t n_ports = ssn_dev_count();
  if (n_ports != n_ports_want) {
    std::string err = slankdev::format("n_ports is not %zd (current %zd)",
        n_ports_want, ssn_dev_count());
    throw slankdev::exception(err.c_str());
  }

  ssn_vnf_port* port0 = new ssn_vnf_port(0, 4, 4); // dpdk0
  ssn_vnf_port* port1 = new ssn_vnf_port(1, 4, 4); // dpdk1
  ssn_vnf_port* port2 = new ssn_vnf_port(2, 4, 4); // dpdk2
  ssn_vnf_port* port3 = new ssn_vnf_port(3, 4, 4); // dpdk3
  printf("\n");
  port0->debug_dump(stdout); printf("\n");
  port1->debug_dump(stdout); printf("\n");
  port2->debug_dump(stdout); printf("\n");
  port3->debug_dump(stdout); printf("\n");

  vnf v0("vnf0");
  v0.attach_port(0, port0);
  v0.attach_port(1, port1);

  vnf v1("vnf1");
  v1.attach_port(0, port2);
  v1.attach_port(1, port3);

  //-------------------------------------------------------

  port0->reset_acc();
  port1->reset_acc();
  v0.set_coremask(0, 0x02); /* 0b00000010:0x02 */
  v0.config_port_acc();
  v0.deploy();

  port2->reset_acc();
  port3->reset_acc();
  v1.set_coremask(0, 0x04); /* 0b00000100:0x04 */
  v1.config_port_acc();
  v1.deploy();


  getchar();
  v0.undeploy();
  v1.undeploy();

fin:
  delete port0;
  delete port1;
  delete port2;
  delete port3;
  ssn_fin();
}


