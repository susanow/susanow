
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


class ssn_vnf_port_dpdk_afpacket : public ssn_vnf_port_dpdk {

  /**
   * @brief get dpdk_port_id of pnic_pmd by pci-address
   * @details
   *   This function probe PMD dynamicaly using DPDK-API.
   */
  static size_t
  vpmd_afpacket(const char* name, const char* ifname, size_t n_que)
  {
    static size_t index = 0; index++;
    std::string devargs = slankdev::format(
        "net_af_packet%zd,iface=%s,qpairs=%zd", index, ifname, n_que);
    size_t pid = dpdk::eth_dev_attach(devargs.c_str());
    ssn_port_stat_init_pid(pid);
    return pid;
  }

 public:

  ssn_vnf_port_dpdk_afpacket(const char* n, const char* ifname, size_t n_que)
    : ssn_vnf_port_dpdk(n, vpmd_afpacket(n, ifname, n_que)) {}

}; /* ssn_vnf_port_dpdk_afpacket */


