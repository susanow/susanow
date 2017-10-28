
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


size_t vpmd_tap(const char* devname)
{
  static size_t index = 0; index++;
  std::string devargs = slankdev::format("net_tap%zd,iface=%s", index, devname);
  return dpdk::eth_dev_attach(devargs.c_str());
}

size_t ppmd_pci(const char* pci_addr_str)
{
  static size_t index = 0; index++;
  std::string devargs = slankdev::format("%s", pci_addr_str);
  return dpdk::eth_dev_attach(devargs.c_str());
}

class ssn_nfvi;
typedef void (*ssn_nfvi_userop_t)(ssn_nfvi*,void*);

class ssn_nfvi final {
 private:

  rte_mempool* mp;
  std::vector<ssn_vnf*> vnfs;
  std::vector<ssn_vnf_port*> ports;

  ssn_nfvi_userop_t userop;
  void* userop_arg;

 public:

  ssn_nfvi(int argc, char** argv) : mp(nullptr), userop(nullptr)
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

  void add_userop(ssn_nfvi_userop_t op, void* arg)
  {
    userop = op;
    userop_arg = arg;
  }

  void reset_userop()
  {
    userop_arg = nullptr;
    userop = nullptr;
  }

  size_t append_vnf(ssn_vnf* vnf)
  {
    vnfs.push_back(vnf);
    return vnfs.size()-1;
  }

  size_t append_vport(ssn_vnf_port* port)
  {
    ports.push_back(port);
    return ports.size()-1;
  }

  void deploy()
  {
    if (userop) {
      userop(this, userop_arg);
    }
  }

  void debug_dump(FILE* fp) const
  {
    printf("\n\n");

    printf("vnfs \n\n");
    printf("  %3s  %-10s  \n", "idx", "name");
    printf(" ------------------------------------------\n");
    const size_t n_vnfs = vnfs.size();
    for (size_t i=0; i<n_vnfs; i++) {
      printf("  %3zd  %-10s \n", i, vnfs[i]->name.c_str());
    }

    printf("\n\n");

    printf("ports \n\n");
    printf("  %3s  %-10s  \n", "idx", "name");
    printf(" ------------------------------------------\n");
    const size_t n_ports = ports.size();
    for (size_t i=0; i<n_ports; i++) {
      printf("  %3zd  %-10s \n", i, ports[i]->name.c_str());
    }

    printf("\n\n");
  }

  struct rte_mempool* get_mp() { return mp; }
  void undeploy() { throw NI("ssn_nfvi::undeploy"); }

}; /* class ssn_nfvi */


