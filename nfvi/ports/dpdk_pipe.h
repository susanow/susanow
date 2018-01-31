
/*
 * MIT License
 *
 * Copyright (c) 2018 Susanow
 * Copyright (c) 2018 Hiroki SHIROKURA
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
#include <ports/dpdk.h>
#include <dpdk/dpdk.h>
#include <slankdev/string.h>
#include <ssn_vnf_port.h>


class ssn_vnf_port_dpdk_pipe : public ssn_vnf_port_dpdk {

  /**
   * @brief get dpdk_port_id of pnic_pmd by pci-address
   * @details
   *   This function probe PMD dynamicaly using DPDK-API.
   */
  static size_t
  vpmd_pipe()
  {
    static size_t index = 0; index++;
    std::string devargs = slankdev::format(
        "net_pipe%zd", index);
    size_t pid = dpdk::eth_dev_attach(devargs.c_str());
    printf("DEBUG id=3532 pid=%zd \n", pid);
    ssn_port_stat_init_pid(pid);
    return pid;
  }

 public:

  ssn_vnf_port_dpdk_pipe(const char* n)
    : ssn_vnf_port_dpdk(n, vpmd_pipe()) {}

}; /* ssn_vnf_port_dpdk_pipe */


class ssn_vnf_port_dpdk_pipeattach : public ssn_vnf_port_dpdk {

  static size_t
  _eth_dev_attach_slank(const char* devargs)
  {
    uint8_t new_pid;
    int ret = rte_eth_dev_attach_slank(devargs, &new_pid);
    if (ret < 0) {
      std::string err = dpdk::format("dpdk::eth_dev_attach (ret=%d)", ret);
      throw dpdk::exception(err.c_str());
    }
    return new_pid;
  }

  static size_t
  vpmd_pipe(const char* remote)
  {
    static size_t index = 15; index--;
    std::string devargs = slankdev::format(
        "net_pipe%zd,attach=%s", index, remote);
    size_t pid = _eth_dev_attach_slank(devargs.c_str());
    printf("DEBUG id=3532 pid=%zd \n", pid);
    ssn_port_stat_init_pid(pid);
    return pid;
  }

 public:

  ssn_vnf_port_dpdk_pipeattach(const char* n, const char* remote)
    : ssn_vnf_port_dpdk(n, vpmd_pipe(remote)) {}

}; /* ssn_vnf_port_dpdk_pipeattach */


