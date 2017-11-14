
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
#include <thread>

#include <ssn_nfvi.h>
#include <ssn_vnf_l2fwd1b.h>
#include <ssn_vnf_l2fwd2b.h>
#include <ssn_rest_api.h>


void user_operation_mock(ssn_nfvi* nfvi) try
{
  rte_mempool* mp = nfvi->get_mp();

  ssn_vnf_port* tap0 = nfvi->port_alloc_tap("tap0", "tap0");
  tap0->config_hw(4, 4);
  ssn_vnf_port* tap1 = nfvi->port_alloc_tap("tap1", "tap1");
  tap1->config_hw(4, 4);
  ssn_vnf_port* virt0 = nfvi->port_alloc_virt("virt0");
  virt0->config_hw(4, 4);
  ssn_vnf_port* virt1 = nfvi->port_alloc_virt("virt1");
  virt1->config_hw(4, 4);

  ssn_vnf* vnf0 = nfvi->vnf_alloc_from_catalog("l2fwd1b", "vnf0");
  vnf0->attach_port(0, tap0);
  vnf0->attach_port(1, virt0);
  ssn_vnf* vnf1 = nfvi->vnf_alloc_from_catalog("l2fwd1b", "vnf1");
  vnf1->attach_port(0, virt1);
  vnf1->attach_port(1, tap1);

  nfvi->ppp_alloc("ppp0", virt0, virt1);
  ssn_vnf_port_patch_panel* ppp0 = nfvi->find_ppp("ppp0");

  vnf0->reset();
  vnf0->set_coremask(0, 4);
  vnf0->deploy();

  vnf1->reset();
  vnf1->set_coremask(0, 8);
  vnf1->deploy();

  printf("user operation was done !\n");

#if 0
  nfvi->vnf_alloc_from_catalog("l2fwd2b", "l2fwd2b-vnf");
  nfvi->port_alloc_tap("tap0", "tap0");
  nfvi->port_alloc_pci("pci0", "0000:01:00.0");
  nfvi->port_alloc_virt("virt0");
#endif

} catch (std::exception& e) { printf("throwed: %s \n", e.what()); }


void contoll(ssn_nfvi* nfvi)
{
  while (true) {
    printf("ssn> ");
    fflush(stdout);
    std::string line;
    std::getline(std::cin, line);
    if (line == "quit") break;
    else if (line == "dump") nfvi->debug_dump(stdout);
  }
  nfvi->stop();
}

int main(int argc, char** argv)
{
  ssn_nfvi nfvi(argc, argv);
  nfvi.vnf_register_to_catalog("l2fwd1b", ssn_vnfalloc_l2fwd1b);
  nfvi.vnf_register_to_catalog("l2fwd2b", ssn_vnfalloc_l2fwd2b);
  nfvi.port_register_to_catalog("pci" , ssn_portalloc_pci );
  nfvi.port_register_to_catalog("tap" , ssn_portalloc_tap );
  nfvi.port_register_to_catalog("virt", ssn_portalloc_virt);

  user_operation_mock(&nfvi);
  std::thread tt(contoll, &nfvi);

  nfvi.run(8888);
  tt.join();
  printf("bye...\n");
}


