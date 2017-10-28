
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

#pragma once


size_t append_tap_pmd(const char* devname)
{
  static size_t index = 0; index++;
  std::string devargs = slankdev::format("net_tap%zd,iface=%s", index, devname);
  return dpdk::eth_dev_attach(devargs.c_str());
}

size_t append_pci_nic(const char* pci_addr_str)
{
  static size_t index = 0; index++;
  std::string devargs = slankdev::format("%s", pci_addr_str);
  return dpdk::eth_dev_attach(devargs.c_str());
}

class ssn_nfvi {
  rte_mempool* mp;

 public:

  ssn_nfvi(int argc, char** argv) : mp(nullptr)
  {
    ssn_init(argc, argv);
    const size_t n_ports = ssn_dev_count();
    for (size_t i=0; i<n_ports; i++) {
      dpdk::eth_dev_detach(i);
    }
    mp = dpdk::mp_alloc("NFVi");
    printf("FINISH nfvi initialization\n\n");
  }

  virtual ~ssn_nfvi()
  {
    rte_mempool_free(mp);
    ssn_fin();
  }

  struct rte_mempool* get_mp() { return mp; }

}; /* class ssn_nvfi */


