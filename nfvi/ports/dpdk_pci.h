
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
#include <dpdk/dpdk.h>
#include <slankdev/string.h>


class ssn_vnf_port_dpdk_pci : public ssn_vnf_port_dpdk {

  /**
   * @brief get dpdk_port_id of pnic_pmd by pci-address
   * @details
   *   This function probe PMD dynamicaly using DPDK-API.
   */
  static size_t
  ppmd_pci(const char* pci_addr_str)
  {
    static size_t index = 0; index++;
    std::string devargs = slankdev::format("%s", pci_addr_str);
    size_t pid = dpdk::eth_dev_attach(devargs.c_str());
    ssn_port_stat_init_pid(pid);
    return pid;
  }

 public:

  /**
   * @brief get pci device adress
   * @return pci-address
   */
  ssn_pci_address get_pci_addr() const
  {
    struct rte_eth_dev_info dev_info;
    const size_t pid = get_dpdk_pid();
    rte_eth_dev_info_get(pid, &dev_info);
    ssn_pci_address addr;
    addr.dom = dev_info.pci_dev->addr.domain;
    addr.bus = dev_info.pci_dev->addr.bus;
    addr.dev = dev_info.pci_dev->addr.devid;
    addr.fun = dev_info.pci_dev->addr.function;
    return addr;
  }

  ssn_vnf_port_dpdk_pci(const char* n, const char* pciaddr)
    : ssn_vnf_port_dpdk(n, ppmd_pci(pciaddr)) {}

}; /* ssn_vnf_port_dpdk_pci */
