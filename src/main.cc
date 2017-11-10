
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


void user_operation_mock(ssn_nfvi* nfvi)
{
  rte_mempool* mp = nfvi->get_mp();

  ssn_portalloc_pci_arg pci0arg = { mp, "0000:01:00.0" };
  nfvi->port_alloc_from_catalog("pci", "pci0", &pci0arg);
  nfvi->find_port("pci0")->config_hw(4, 4);

  ssn_portalloc_pci_arg pci1arg = { mp, "0000:01:00.1" };
  nfvi->port_alloc_from_catalog("pci", "pci1", &pci1arg);
  nfvi->find_port("pci1")->config_hw(4, 4);

  ssn_portalloc_tap_arg tap0arg = { mp, "tap0" };
  nfvi->port_alloc_from_catalog("tap", "tap0", &tap0arg);
  nfvi->find_port("tap0")->config_hw(4, 4);

  ssn_portalloc_tap_arg tap1arg = { mp, "tap1" };
  nfvi->port_alloc_from_catalog("tap", "tap1", &tap1arg);
  nfvi->find_port("tap1")->config_hw(4, 4);

  nfvi->vnf_alloc_from_catalog("l2fwd1b", "vnf0");
  nfvi->vnf_alloc_from_catalog("l2fwd1b", "vnf1");

  ssn_vnf* vnf;

  vnf = nfvi->find_vnf("vnf0");
  vnf->attach_port(0, nfvi->find_port("pci0")); // not yet
  vnf->attach_port(1, nfvi->find_port("pci1")); // not yet
  vnf->reset_allport_acc();
  vnf->set_coremask(0, 0b00000100);
  vnf->deploy();

  vnf = nfvi->find_vnf("vnf1");
  vnf->attach_port(0, nfvi->find_port("tap0")); // not yet
  vnf->attach_port(1, nfvi->find_port("tap1")); // not yet
  vnf->reset_allport_acc();
  vnf->set_coremask(0, 0b00001000);
  vnf->deploy();
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
  nfvi.debug_dump(stdout);
  std::thread rest_api(rest_api_thread, &nfvi);

  getchar();
  nfvi.undeploy_all_vnfs();
  rest_api.join();
}


