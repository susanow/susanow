
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
#include <ssn_vnf_l2fwd2b.h>
#include <ssn_vnf_port_dpdk.h>

int main(int argc, char** argv)
{
  ssn_init(argc, argv);
  constexpr size_t n_ports_wanted = 2;
  size_t n_ports = ssn_dev_count();
  if (n_ports != n_ports_wanted) {
    std::string err = slankdev::format("n_ports is not %zd (current %zd)",
        n_ports_wanted, ssn_dev_count());
    throw slankdev::exception(err.c_str());
  }

  //----------------------------------------------------------

  rte_mempool* mp = dpdk::mp_alloc("ssn");
  ssn_vnf_port_dpdk dpdk0("dpdk0", 0, mp);
  ssn_vnf_port_dpdk dpdk1("dpdk1", 1, mp);
  dpdk0.config_hw(4, 4);
  dpdk1.config_hw(4, 4);

  //----------------------------------------------------------

  ssn_vnf_l2fwd2b v0("vnf0");
  v0.attach_port(0, &dpdk0);
  v0.attach_port(1, &dpdk1);

  //----------------------------------------------------------

  v0.reset();
  v0.set_coremask(0, 0x02); /* 0b00000010:0x02 */
  v0.set_coremask(1, 0x04); /* 0b00000100:0x04 */
  v0.deploy();
  v0.debug_dump(stdout);
  getchar();
  v0.undeploy();

  //----------------------------------------------------------

  v0.reset();
  v0.set_coremask(0, 0x06); /* 0b00000110:0x06 */
  v0.set_coremask(1, 0x18); /* 0b00011000:0x18 */
  v0.deploy();
  v0.debug_dump(stdout);
  getchar();
  v0.undeploy();

  //----------------------------------------------------------

  rte_mempool_free(mp);
  ssn_fin();
}


