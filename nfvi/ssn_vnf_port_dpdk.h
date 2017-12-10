
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
#include <ssn_vnf_port.h>

class ssn_vnf;

/**
 * @brief get dpdk_port_id of tap_pmd by defice-name
 * @details
 *   This function probe vPMD dynamicaly using DPDK-API.
 */
inline size_t
vpmd_tap(const char* devname)
{
  static size_t index = 0; index++;
  std::string devargs = slankdev::format("net_tap%zd,iface=%s", index, devname);
  size_t pid = dpdk::eth_dev_attach(devargs.c_str());
  ssn_port_stat_init_pid(pid);
  return pid;
}

/**
 * @brief get dpdk_port_id of pnic_pmd by pci-address
 * @details
 *   This function probe PMD dynamicaly using DPDK-API.
 */
inline size_t
ppmd_pci(const char* pci_addr_str)
{
  static size_t index = 0; index++;
  std::string devargs = slankdev::format("%s", pci_addr_str);
  size_t pid = dpdk::eth_dev_attach(devargs.c_str());
  ssn_port_stat_init_pid(pid);
  return pid;
}

struct ssn_pci_address {
  uint32_t dom;
  uint8_t  bus;
  uint8_t  dev;
  uint8_t  fun;

  void set(const char* str)
  {
    uint32_t dom_;
    uint32_t bus_;
    uint32_t dev_;
    uint32_t fun_;
    int ret = sscanf(str, "%04x:%02x:%02x.%01x", &dom_, &bus_, &dev_, &fun_);
    if (ret != 4) {
      printf("ret: %d \n", ret);
      printf("  dom: %04x \n", dom_);
      printf("  bus: %02x \n", bus_);
      printf("  dev: %02x \n", dev_);
      printf("  fun: %01x \n", fun_);
      throw slankdev::exception("OKASHII 12dfd3");
    }

    dom = dom_;
    bus = bus_;
    dev = dev_;
    fun = fun_;
  }

  std::string str() const
  {
    std::string str = "";
    str = slankdev::format("%04x:%02x:%02x.%01X", dom, bus, dev, fun);
    return str;
  }
};

/**
 * @brief vnf port class specilized for DPDK-pmd
 * @details
 *   This class is one of special-class that inherrit ssn_vnf_port class.
 */
class ssn_vnf_port_dpdk : public ssn_vnf_port {

  const size_t port_id; /*! dpdk port id */
  size_t irx_pps_sum;
  size_t irx_pps_cur;
  rte_mempool* mp;

 public:

  /**
   * @brief get dpdk's port-id
   * @return dpdk_pid
   */
  size_t get_dpdk_pid() const { return port_id; }

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

  /**
   * @brief get device's socket-id
   * @return socket-id. 0,1,2...
   */
  virtual size_t get_socket_id() const override { return rte_eth_dev_socket_id(port_id); }

  /**
   * @brief constructor
   * @param [in] a_port_id dpdk port id
   * @param [in] a_n_rxq number of rx queues for RSS multiqueues
   * @param [in] a_n_txq number of tx queues for RSS multiqueues
   * @details
   *   User must specilize following information when construct this instance.
   *   - dpdk port id
   *   - num of rx queues (hardware multiqueues)
   *   - num of tx queues (hardware multiqueues)
   */
  ssn_vnf_port_dpdk(const char* n, size_t a_port_id)
    : ssn_vnf_port(n), port_id(a_port_id),
    irx_pps_sum(0), irx_pps_cur(0), mp(nullptr) {}

  void set_mp(rte_mempool* m) { mp = m; }

  virtual bool deployable() const override { return true; }

  virtual void config_hw(size_t nrxq, size_t ntxq) override
  {
    this->n_rxq = nrxq;
    this->n_txq = ntxq;
    ssn_ma_port_configure_hw(port_id, nrxq, ntxq, mp);
    ssn_ma_port_dev_up(port_id);
    ssn_ma_port_promisc_on(port_id);
  }

  /**
   * @brief Send a burst of output packets of an Ethernet device
   * @param [in] aid Access ID
   * @param [in] mbufs The address of an array of nb_pkts pointers
   *             to rte_mbuf structures which contain the output packets.
   * @param [in] n_mbufs The maximum number of packets to transmit.
   * @details
   *   User need to configure ssn_vnf_port instance with configure_acc()
   *   before of calling this function.
   *   This function calls ssn_ma_port_tx_burst internally
   */
  virtual size_t
  tx_burst(size_t aid, rte_mbuf** mbufs, size_t n_mbufs) override
  { return ssn_ma_port_tx_burst(port_id, aid, mbufs, n_mbufs); }

  /**
   * @brief Receive a burst of output packets from an Ethernet device
   * @param [in] aid Access ID
   * @param [in] mbufs The address of an array of pointers to rte_mbuf
   *             structures that must be large enough to store nb_pkts
   *             pointers in it.
   * @param [in] n_mbufs The maximum number of packets to retrieve
   * @details
   *   User need to configure ssn_vnf_port instance with configure_acc()
   *   before of calling this function.
   *   This function calls ssn_ma_port_rx_burst internally
   */
  virtual size_t
  rx_burst(size_t aid, rte_mbuf** mbufs, size_t n_mbufs) override
  {
    size_t ret = ssn_ma_port_rx_burst(port_id, aid, mbufs, n_mbufs);
    irx_pps_sum += ret;
    return ret;
  }

  /**
   * @brief Debug output
   * @param fp FILE* file pointer to output
   */
  virtual void debug_dump(FILE* fp) const override
  {
    fprintf(fp, " port_id: %zd\r\n", port_id);
    fprintf(fp, " n_rxq  : %zd\r\n", n_rxq  );
    fprintf(fp, " n_txq  : %zd\r\n", n_txq  );
    fprintf(fp, " n_rxacc: %zd\r\n", n_rxacc);
    fprintf(fp, " n_txacc: %zd\r\n", n_txacc);
  }

  /**
   * @brief Configure form accessors
   * @details
   *   This configuration can be changed while running dynamicaly
   *   for Susanow Dynamic Auto NF Optimization.
   *   This function calls ssn_ma_port_configure_acc internally
   */
  virtual void config_acc() override
  { ssn_ma_port_configure_acc(port_id, n_rxacc, n_txacc); }

  /**
   * @brief get "inner rx perf"
   * @return return packet/seconds performance [Mpps]
   */
  virtual size_t get_inner_rx_perf() const override
  { return irx_pps_cur; }

  /**
   * @brief get "inner tx perf"
   * @return return packet/seconds performance [Mpps]
   */
  virtual size_t get_inner_tx_perf() const override
  {
    size_t ret = 0;
    ret += ssn_port_stat_get_cur_tx_pps(port_id);
    return ret;
  }

  /**
   * @brief get "outer rx perf"
   * @return return packet/seconds performance [Mpps]
   */
  virtual size_t get_outer_rx_perf() const override
  {
    size_t ret = 0;
    ret += ssn_port_stat_get_cur_rx_pps(port_id);
    ret += ssn_port_stat_get_cur_rx_mis(port_id);
    return ret;
  }

  /**
   * @brief get "outer tx perf"
   * @return return packet/seconds performance [Mpps]
   */
  virtual size_t get_outer_tx_perf() const override
  { return ssn_port_stat_get_cur_tx_pps(port_id); }

  /**
   * @brief update port-statistics for timer-function.
   * @details
   *   This function must be called once a second.
   */
  virtual void stats_update_per1sec() override
  {
    irx_pps_cur = irx_pps_sum;
    irx_pps_sum = 0;
  }

}; /* class ssn_vnf_port_dpdk */



/**
 * @brief for only ssn_portalloc_tap()
 */
struct ssn_portalloc_tap_arg {
  rte_mempool* mp;
  std::string linux_ifname;
}; /* struct ssn_portalloc_tap_arg */

/**
 * @brief port-allocator for ssn_port_catalog
 * @details
 *    This function allocate ssn_vnf_port_dpdk class of tap_pmd
 *    from dpdk-mempool, linux-netifname and alias-name
 */
inline ssn_vnf_port*
ssn_portalloc_tap(const char* instance_name, void* arg)
{
  ssn_portalloc_tap_arg* s = reinterpret_cast<ssn_portalloc_tap_arg*>(arg);
  rte_mempool* mp = s->mp;
  std::string ifname = s->linux_ifname;
  auto port = new ssn_vnf_port_dpdk(instance_name, vpmd_tap(ifname.c_str()));
  port->set_mp(mp);
  return port;
}


/**
 * @brief for only ssn_portalloc_pci()
 */
struct ssn_portalloc_pci_arg {
  rte_mempool* mp;
  std::string pci_addr;
}; /* struct ssn_portalloc_pci_arg */

/**
 * @brief port-allocator for ssn_port_catalog
 * @details
 *    This function allocate ssn_vnf_port_dpdk class of pNIC
 *    from dpdk-mempool, pci-address and alias-name
 */
inline ssn_vnf_port*
ssn_portalloc_pci(const char* instance_name, void* arg)
{
  ssn_portalloc_pci_arg* s = reinterpret_cast<ssn_portalloc_pci_arg*>(arg);
  rte_mempool* mp = s->mp;
  std::string pci_addr = s->pci_addr;
  auto port = new ssn_vnf_port_dpdk(instance_name, ppmd_pci(pci_addr.c_str()));
  port->set_mp(mp);
  return port;
}
