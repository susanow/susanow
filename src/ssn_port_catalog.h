
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

#include <ssn_types.h>
#include <ssn_vnf_v02.h>

class ssn_vnf_port;
typedef ssn_vnf_port*(*ssn_portallocfunc_t)(const char* instance_name, void* arg);

inline size_t vpmd_tap(const char* devname)
{
  static size_t index = 0; index++;
  std::string devargs = slankdev::format("net_tap%zd,iface=%s", index, devname);
  size_t pid = dpdk::eth_dev_attach(devargs.c_str());
  ssn_port_stat_init_pid(pid);
  return pid;
}

inline size_t ppmd_pci(const char* pci_addr_str)
{
  static size_t index = 0; index++;
  std::string devargs = slankdev::format("%s", pci_addr_str);
  size_t pid = dpdk::eth_dev_attach(devargs.c_str());
  ssn_port_stat_init_pid(pid);
  return pid;
}

struct ssn_portalloc_pci_arg {
  rte_mempool* mp;
  std::string pci_addr;
}; /* struct ssn_portalloc_pci_arg */
inline ssn_vnf_port*
ssn_portalloc_pci(const char* instance_name, void* arg)
{
  ssn_portalloc_pci_arg* s = reinterpret_cast<ssn_portalloc_pci_arg*>(arg);
  rte_mempool* mp = s->mp;
  std::string pci_addr = s->pci_addr;
  return new ssn_vnf_port_dpdk("pci0", ppmd_pci(pci_addr.c_str()), mp);
}

struct ssn_portalloc_tap_arg {
  rte_mempool* mp;
  std::string linux_ifname;
}; /* struct ssn_portalloc_tap_arg */

inline ssn_vnf_port*
ssn_portalloc_tap(const char* instance_name, void* arg)
{
  ssn_portalloc_tap_arg* s = reinterpret_cast<ssn_portalloc_tap_arg*>(arg);
  rte_mempool* mp = s->mp;
  std::string ifname = s->linux_ifname;
  return new ssn_vnf_port_dpdk("tap0", vpmd_tap(ifname.c_str()), mp);
}

struct ssn_portalloc_virt_arg {
}; /* struct ssn_portalloc_virt_arg */

inline ssn_vnf_port*
ssn_portalloc_virt(const char* instance_name, void* nouse)
{ return new ssn_vnf_port_virt(instance_name); }


/**
 * @brief vnf_port catalog class
 * @details
 *   This class provides port-allocationing to dynamic vnf using
 */
class ssn_port_catalog final {

  struct catalog_ele {
    std::string name;
    ssn_portallocfunc_t allocator;
    catalog_ele(std::string n, ssn_portallocfunc_t f) : name(n), allocator(f) {}
  }; /* struct catalog_ele */

  std::vector<catalog_ele> catalog;

 public:

  ssn_vnf_port* alloc_port(const char* catalog_name,
                const char* instance_name, void* arg)
  {
    size_t n_cat = catalog.size();
    for (size_t i=0; i<n_cat; i++) {
      if (catalog[i].name == catalog_name)
        return catalog[i].allocator(instance_name, arg);
    }
    throw slankdev::exception("ssn_port_catalog::alloc_port: not found port");
  }

  void register_port(const char* catalog_name, ssn_portallocfunc_t allocator)
  { catalog.emplace_back(catalog_name, allocator); }

  void unregister_port(const char* catalog_name)
  {
    size_t n_cat = catalog.size();
    for (size_t i=0; i<n_cat; i++) {
      if (catalog[i].name == catalog_name) {
        catalog.erase(catalog.begin() + i);
        return ;
      }
    }
    throw slankdev::exception("ssn_port_catalog::register_port: not found port");
  }

}; /* class ssn_port_catalog */


