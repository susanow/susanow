
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
#include <slankdev/string.h>


int main(int argc, char** argv)
{
  ssn_init(argc, argv);
  rte_mempool* mp = dpdk::mp_alloc("ssn");
  ssn_vnf_port_dpdk* dpdk[100];

  const size_t n_port = ssn_dev_count();
  for (size_t i=0; i<n_port; i++) {
    auto name = slankdev::format("dpdk%zd", i);
    dpdk[i] = new ssn_vnf_port_dpdk(name.c_str(), i, mp);
    dpdk[i]->config_hw(4,4);
  }
  printf("\n\n");

  //-------------------------------------------------------

  for (size_t i=0; i<n_port; i++) {
    printf("port%zd\n", i);
    printf("pci: %s \n", dpdk[i]->get_pci_addr().str().c_str());
    printf("socket-id: %zd \n", dpdk[i]->get_socket_id());
    printf("\n");
  }

  //-------------------------------------------------------

  printf("\n");
  for (size_t i=0; i<n_port; i++) delete dpdk[i];
  rte_mempool_free(mp);
  ssn_fin();
}


