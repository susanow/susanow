
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
#include <slankdev/util.h>
#include <slankdev/string.h>
#include <dpdk/dpdk.h>

#include <ssn_port.h>
#include <ssn_common.h>
#include <ssn_log.h>
#include <ssn_vnf_v02_l2fwd1b.h>
#include "ssn_nfvi.h"


class labnet_nfvi : public ssn_nfvi {
 private:
  ssn_vnf* vnf0;
  ssn_vnf* vnf1;
  ssn_vnf_port* tap0;
  ssn_vnf_port* tap1;
  ssn_vnf_port* pci0;
  ssn_vnf_port* pci1;

 public:
  labnet_nfvi(int argc, char** argv) : ssn_nfvi(argc, argv)
  {
    rte_mempool* mp = this->get_mp();

    tap0 = new ssn_vnf_port_dpdk("tap0", ppmd_pci("0000:01:00.0"), 4, 4, mp);
    tap1 = new ssn_vnf_port_dpdk("tap1", ppmd_pci("0000:01:00.1"), 4, 4, mp);
    pci0 = new ssn_vnf_port_dpdk("pci0", vpmd_tap("tap0"        ), 4, 4, mp);
    pci1 = new ssn_vnf_port_dpdk("pci1", vpmd_tap("tap1"        ), 4, 4, mp);
    this->append_vport(tap0);
    this->append_vport(tap1);
    this->append_vport(pci0);
    this->append_vport(pci1);

    vnf0 = new ssn_vnf_l2fwd1b("vnf0");
    vnf1 = new ssn_vnf_l2fwd1b("vnf1");
    this->append_vnf(vnf0);
    this->append_vnf(vnf1);
  }

  ~labnet_nfvi()
  {
    delete vnf0;
    delete vnf1;
    delete tap0;
    delete tap1;
    delete pci0;
    delete pci1;
  }

  virtual void deploy() override
  {
    ssn_vnf* vnf;
    vnf = this->find_vnf("vnf0");
    vnf->attach_port(0, this->find_port("pci0"));
    vnf->attach_port(1, this->find_port("pci1"));
    vnf->reset_allport_acc();
    vnf->set_coremask(0, 0b00000010);
    vnf->deploy();

    vnf = this->find_vnf("vnf1");
    vnf->attach_port(0, this->find_port("tap0"));
    vnf->attach_port(1, this->find_port("tap1"));
    vnf->reset_allport_acc();
    vnf->set_coremask(0, 0b00000100);
    vnf->deploy();

    this->debug_dump(stdout);
  }
}; /* class nfvi */


int main(int argc, char** argv)
{
  labnet_nfvi nfvi0(argc, argv);
  nfvi0.deploy();
  getchar();
  nfvi0.undeploy_all_vnfs();
}


