
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
#include <ssn_ma_port.h>
#include <ssn_vnf_v02.h>
#include <slankdev/vector.h>
#include <ssn_vnf_v02_l2fwd2b.h>


int main(int argc, char** argv)
{
  constexpr size_t n_ports_wanted = 2;
  constexpr size_t n_rxq = 8;
  constexpr size_t n_txq = 8;

  ssn_init(argc, argv);
  size_t n_ports = ssn_dev_count();
  if (n_ports != n_ports_wanted) {
    std::string err = slankdev::format("n_ports is not %zd (current %zd)",
        n_ports_wanted, ssn_dev_count());
    throw slankdev::exception(err.c_str());
  }

  rte_mempool* mp = dpdk::mp_alloc("ssn");
  ssn_vnf_port* port[4];
  port[0] = new ssn_vnf_port_dpdk("dpdk0", 0, 4, 4, mp); // dpdk0
  port[1] = new ssn_vnf_port_dpdk("dpdk1", 1, 4, 4, mp); // dpdk1

  /*--------deploy-field-begin----------------------------------------------*/

  printf("\n");
  ssn_vnf_l2fwd2b v0("vnf0");
  v0.attach_port(0, port[0]);
  v0.attach_port(1, port[1]);

  //----------------------------------------------------------

  v0.reset_allport_acc();
  v0.set_coremask(0, 0x02); /* 0b00000010:0x02 */
  v0.set_coremask(1, 0x04); /* 0b00000100:0x04 */
  v0.deploy();
  v0.debug_dump(stdout);

  //----------------------------------------------------------

  getchar();
  v0.undeploy();

  //----------------------------------------------------------

  v0.reset_allport_acc();
  v0.set_coremask(0, 0x06); /* 0b00000110:0x06 */
  v0.set_coremask(1, 0x18); /* 0b00011000:0x18 */
  v0.deploy();
  v0.debug_dump(stdout);

  //----------------------------------------------------------

  getchar();
  v0.undeploy();

  /*--------deploy-field-end------------------------------------------------*/

  rte_mempool_free(mp);
  delete port[0];
  delete port[1];
  ssn_fin();
}


