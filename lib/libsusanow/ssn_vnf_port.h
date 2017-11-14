
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
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <slankdev/string.h>
#include <slankdev/exception.h>
#include <slankdev/util.h>
#include <exception>

#include <ssn_port_stat.h>
#include <ssn_ma_port.h>
#include <ssn_ma_ring.h>
#include <ssn_thread.h>
#include <ssn_cpu.h>
#include <ssn_port.h>
#include <ssn_common.h>
#include <ssn_log.h>
#include <dpdk/dpdk.h>
#include <slankdev/vector.h>

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


/**
 * @brief vnf port class
 * @details
 *   Now, this class implement for only dpdk-phy-port. but we are going to
 *   extend this class to support virtual-port for NF-chaining.
 *   This class is abstract class.
 *   There are some classes inherrit this,
 *   ssn_vnf_port_dpdk and ssn_vnf_port_virt.
 */
class ssn_vnf_port {

 protected:

  ssn_vnf* attached_vnf;
  size_t n_rxq;         /*! num of rx queues (hardware) */
  size_t n_txq;         /*! num of tx queues (hardware) */
  size_t n_rxacc;       /*! num of rx accessor */
  size_t n_txacc;       /*! num of tx accessor */

 public:
  const std::string name;

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
  ssn_vnf_port(const char* n)
    : attached_vnf(nullptr), n_rxq(0), n_txq(0), n_rxacc(0), n_txacc(0), name(n) {}

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
  virtual size_t tx_burst(size_t aid, rte_mbuf** mbufs, size_t n_mbufs) = 0;

  virtual bool deployable() const = 0;
  bool is_attached_vnf() const { return attached_vnf!=nullptr; }
  const ssn_vnf* get_attached_vnf() const { return attached_vnf; }
  void attach_vnf(ssn_vnf* vnf) { attached_vnf = vnf; }
  void dettach_vnf() { attached_vnf = nullptr; }

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
  virtual size_t rx_burst(size_t aid, rte_mbuf** mbufs, size_t n_mbufs) = 0;

  /**
   * @brief Debug output
   * @param fp FILE* file pointer to output
   */
  virtual void debug_dump(FILE* fp) const = 0;

  /**
   * @brief Configure form accessors
   * @details
   *   This configuration can be changed while running dynamicaly
   *   for Susanow Dynamic Auto NF Optimization.
   *   This function calls ssn_ma_port_configure_acc internally
   */
  virtual void config_acc() = 0;

  /**
   * @brief Configure HW
   * @details
   *   Calling this function configs Physical-Queues.
   *   ex. when PCI-NIC, this function decide number of hw-queues.
   */
  virtual void config_hw(size_t nrxq, size_t ntxq) = 0;

  /*
   * @brief Rx acccess request
   * @return Rx Access ID
   * @details
   *   Accessor must call this function and this function return.
   *   access-id. Accessor can rx-access via this access-id.
   */
  size_t request_rx_access()
  {
    auto tmp = n_rxacc;
    n_rxacc += 1;
    return tmp;
  }

  /*
   * @brief Tx acccess request
   * @return Tx Access ID
   * @details
   *   Accessor must call this function and this function return.
   *   access-id. Accessor can tx-access via this access-id.
   */
  size_t request_tx_access()
  {
    auto tmp = n_txacc;
    n_txacc += 1;
    return tmp;
  }

  /**
   * @brief Reset number of Rx/Tx Accessors (Both rx and tx).
   * @details
   *   This function only operate number of accessor,
   *   not operate number of queues.
   */
  void reset_acc() { n_rxacc = 0; n_txacc = 0; }

  /**
   * @brief Get number of Rx Queues
   * @return number of rx queues (indepenent access number)
   */
  size_t get_n_rxq() const { return n_rxq; }

  /**
   * @brief Get number of Tx Queues
   * @return number of tx queues (not access number)
   */
  size_t get_n_txq() const { return n_txq; }

  /**
   * @brief Get number of Accessors for Rx
   * @return number of rx accessors (not number of queue)
   */
  size_t get_n_rxacc() const { return n_rxacc; }

  /**
   * @brief Get number of Accessors for Tx
   * @return number of tx accessors (not number of queue)
   */
  size_t get_n_txacc() const { return n_txacc; }

  /*
   * 4 kinds of performance of each-ports
   *
   *         3                1            +---------------+
   *        --->  +-------+  --->         / rxburst         \
   * [traffic]    ||}     |    [process] {      Threads      }
   *        <---  +-------+  <---         \ txburst         /
   *         4                2            +---------------+
   *
   *     1: inner rx perf        : can get rx_burst call-info
   *     2: inner tx perf (==4)  : can get rx_burst call-info
   *     3: outer rx perf        : can get from ethdev-api
   *     4: outer tx perf (==2)  : can get from ethdev-api
   */
  /*
   * for Virtual Port (patch panel port)
   *
   *
   *         7             5              3             1
   *       ---> +-------+ --->  {ring}  ---> +-------+ --->
   * [process]  |     {||    [patchpanel]    ||}     |  [process]
   *       <--- +-------+ <---  {ring}  <--- +-------+ <---
   *         8             6              4             2
   *            Left Port                   Right Port
   *
   *     Right Port
   *       1: inner rx perf
   *       2: inner tx perf (==4)
   *       3: outer rx perf
   *       4: outer tx perf (==2)
   *     Left Port
   *       5: outer tx perf (==7)
   *       6: outer rx perf
   *       7: inner tx perf (==5)
   *       8: inner rx perf
   */

  /**
   * @brief get "inner rx perf"
   * @return return packet/seconds performance [Mpps]
   */
  virtual size_t get_inner_rx_perf() const = 0;

  /**
   * @brief get "inner tx perf"
   * @return return packet/seconds performance [Mpps]
   */
  virtual size_t get_inner_tx_perf() const = 0;

  /**
   * @brief get "outer rx perf"
   * @return return packet/seconds performance [Mpps]
   */
  virtual size_t get_outer_rx_perf() const = 0;

  /**
   * @brief get "outer tx perf"
   * @return return packet/seconds performance [Mpps]
   */
  virtual size_t get_outer_tx_perf() const = 0;

  /**
   * @brief get "performance reduction" of port
   * @return 0-1 range. if working wirerate, return 1.
   * @details
   *    Working this function need to implement get_inner_rx_perf()
   *    and get_outer_rx_perf(). This value will be used for D2engine.
   */
  double get_perf_reduction() const
  {
    size_t irx = get_inner_rx_perf();
    size_t orx = get_outer_rx_perf();
    if (orx == 0) return 1.0;
    double ret = double(irx)/double(orx);
    return ret;
  }

  virtual void stats_update_per1sec() = 0;

}; /* class ssn_vnf_port */





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
  ssn_vnf_port_dpdk(const char* n, size_t a_port_id, struct rte_mempool* mp_)
    : ssn_vnf_port(n), port_id(a_port_id),
    irx_pps_sum(0), irx_pps_cur(0), mp(mp_) {}

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
  { throw NI("get_inner_tx_perf"); }

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
  return new ssn_vnf_port_dpdk(instance_name, ppmd_pci(pci_addr.c_str()), mp);
}

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
  return new ssn_vnf_port_dpdk(instance_name, vpmd_tap(ifname.c_str()), mp);
}






/**
 * @brief vnf port class specilized for Virt-phy, for VNF-chaining.
 * @details
 *   This class is one of special-class that inherrit ssn_vnf_port class.
 *   ssn_vnf_port_virt does not work itselfs.
 *   This class work when there is a pair of this classes.
 *   port patch panel is provided by ssn_vnf_port_patch_panel class.
 */
class ssn_vnf_port_virt : public ssn_vnf_port {

  friend class ssn_vnf_port_patch_panel;
  ssn_ma_ring* tx;
  ssn_ma_ring* rx;

 public:

  /**
   * @brief constructor
   * @param [in] n_rxq_ number of rx queues for RSS multiqueues
   * @param [in] n_txq_ number of tx queues for RSS multiqueues
   */
  ssn_vnf_port_virt(const char* n)
    : ssn_vnf_port(n), tx(nullptr), rx(nullptr) {}

  virtual bool deployable() const override { return patched(); }

  virtual void config_hw(size_t nrxq, size_t ntxq) override
  {
    this->n_rxq = nrxq;
    this->n_txq = ntxq;
  }

  bool patched() const
  {
    assert((tx==nullptr) == (rx==nullptr));
    return tx!=nullptr;
  }

  /**
   * @brief Send a burst of output packets of an Ethernet device
   *        ( ssn_vnf_port::tx_burst() )
   * @param [in] aid Access ID
   * @param [in] mbufs The address of an array of nb_pkts pointers
   *             to rte_mbuf structures which contain the output packets.
   * @param [in] n_mbufs The maximum number of packets to transmit.
   */
  virtual size_t tx_burst(size_t aid, rte_mbuf** mbufs, size_t n_mbufs) override
  {
    size_t ret = 0;
    ret = tx->enqueue_burst(aid, (void**)mbufs, n_mbufs);
    return ret;
  }

  /**
   * @brief Receive a burst of output packets from an Ethernet device
   *        ( ssn_vnf_port::rx_burst() )
   * @param [in] aid Access ID
   * @param [in] mbufs The address of an array of pointers to rte_mbuf
   *             structures that must be large enough to store nb_pkts
   *             pointers in it.
   * @param [in] n_mbufs The maximum number of packets to retrieve
   */
  virtual size_t rx_burst(size_t aid, rte_mbuf** mbufs, size_t n_mbufs) override
  {
    size_t ret = 0;
    ret = rx->dequeue_burst(aid, (void**)mbufs, n_mbufs);
    return ret;
  }

  /**
   * @brief Debug output
   * @param fp FILE* file pointer to output
   */
  virtual void debug_dump(FILE* fp) const override
  {
    fprintf(fp, " rxqs_ptr: %p\r\n", rx);
    fprintf(fp, " txqs_ptr: %p\r\n", tx);
    fprintf(fp, " n_rxq   : %zd\r\n", n_rxq  );
    fprintf(fp, " n_txq   : %zd\r\n", n_txq  );
    fprintf(fp, " n_rxacc : %zd\r\n", n_rxacc);
    fprintf(fp, " n_txacc : %zd\r\n", n_txacc);
  }

  /**
   * @brief Configure form accessors
   * @details
   *   This configuration can be changed while running dynamicaly
   *   for Susanow Dynamic Auto NF Optimization.
   *   This function calls ssn_ma_port_configure_acc internally
   */
  virtual void config_acc() override
  {
    rx->configure_deq_acc(get_n_rxacc());
    tx->configure_enq_acc(get_n_txacc());
  }

  virtual size_t get_inner_rx_perf() const override
  {
    if (rx) return rx->get_cons_perf();
    else    return 0;
  }

  virtual size_t get_inner_tx_perf() const override
  {
    if (tx) return tx->get_prod_perf();
    else    return 0;
  }

  virtual size_t get_outer_rx_perf() const override
  {
    if (rx) return rx->get_prod_perf();
    else    return 0;
  }

  virtual size_t get_outer_tx_perf() const override
  {
    if (tx) return tx->get_cons_perf();
    else    return 0;
  }

  /**
   * @brief update port-statistics for timer-function.
   * @details
   *   This function must be called once a second.
   */
  virtual void stats_update_per1sec() override
  {
    if (rx)
      rx->update_stats();
  }

}; /* class ssn_vnf_port_virt */




/**
 * @brief for only ssn_portalloc_virt()
 */
struct ssn_portalloc_virt_arg {
}; /* struct ssn_portalloc_virt_arg */

/**
 * @brief port-allocator for ssn_port_catalog
 * @details
 *    This function allocate ssn_vnf_port_dpdk class of ssn_virt_port
 *    from alias-name
 */
inline ssn_vnf_port*
ssn_portalloc_virt(const char* instance_name, void* nouse)
{ return new ssn_vnf_port_virt(instance_name); }





/**
 * @brief Provide 1 patch panel between a pair of ssn_vnf_port_virts
 * @details
 *   This class was designed for NFV/SFC (Service Function Chaining).
 *   This class connect a pair of ssn_vnf_port_virt classes.
 */
class ssn_vnf_port_patch_panel {

  ssn_ma_ring left_enq;
  ssn_ma_ring right_enq;

  ssn_vnf_port_virt* left;
  ssn_vnf_port_virt* right;

 public:
  const std::string name;
  const ssn_vnf_port_virt* get_left () const { return left;  }
  const ssn_vnf_port_virt* get_right() const { return right; }
  bool deletable() const;

  /**
   * @brief constructor
   * @param [in] r right-side of port's pointer
   * @param [in] l left-side of port's pointer
   */
  ssn_vnf_port_patch_panel(const char* n, ssn_vnf_port* r, ssn_vnf_port* l)
    : name(n)
    , left_enq (slankdev::format("%sLeftEnq" , n).c_str())
    , right_enq(slankdev::format("%sRightEnq", n).c_str())
    , right( dynamic_cast<ssn_vnf_port_virt*>(r) )
    , left ( dynamic_cast<ssn_vnf_port_virt*>(l) )
  {
    if (!(r && l)) {
      std::string err = "ssn_vnf_port_patch_panel::ssn_vnf_port_patch_panel: ";
      err += "invalid argument (nullptr)";
      throw slankdev::exception(err.c_str());
    }

    bool c0 = left->get_n_rxq() == right->get_n_txq();
    bool c1 = left->get_n_txq() == right->get_n_rxq();
    if (!(c0 && c1)) {
      std::string err = "ssn_vnf_port_patch_panel::ssn_vnf_port_patch_panel: ";
      err += "patch panel is not support ";
      err += "(n_rxq,n_txq are invalid)";
      throw slankdev::exception(err.c_str());
    }
    size_t n_left_enq  = left->get_n_txq();
    size_t n_right_enq = right->get_n_txq();

    left_enq.configure_que(n_left_enq);
    right_enq.configure_que(n_right_enq);

    left->tx  = &left_enq;
    left->rx  = &right_enq;
    right->tx = &right_enq;
    right->rx = &left_enq;
  }

  /**
   * @brief destructor
   */
  virtual ~ssn_vnf_port_patch_panel()
  {
    left->tx = nullptr;
    left->rx = nullptr;
    right->tx = nullptr;
    right->rx = nullptr;
  }

}; /* class ssn_vnf_port_patch_panel */


