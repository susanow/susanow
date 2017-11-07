
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

  const size_t n_rxq;   /*! num of rx queues (hardware) */
  const size_t n_txq;   /*! num of tx queues (hardware) */
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
  ssn_vnf_port(const char* n, size_t i_n_rxq, size_t i_n_txq)
    : n_rxq(i_n_rxq), n_txq(i_n_txq), n_rxacc(0), n_txacc(0), name(n) {}

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
    double ret = double(irx)/double(orx);
    if (orx == 0) return 1.0;
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
  ssn_vnf_port_dpdk(const char* n, size_t a_port_id,
      size_t a_n_rxq, size_t a_n_txq, struct rte_mempool* mp) :
    ssn_vnf_port(n, a_n_rxq, a_n_txq), port_id(a_port_id),
    irx_pps_sum(0), irx_pps_cur(0)
  {
    ssn_ma_port_configure_hw(port_id, n_rxq, n_txq, mp);
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
  ssn_vnf_port_virt(const char* n, size_t n_rxq_, size_t n_txq_)
    : ssn_vnf_port(n, n_rxq_, n_txq_) {}

  /**
   * @brief Send a burst of output packets of an Ethernet device ( ssn_vnf_port::tx_burst() )
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
   * @brief Receive a burst of output packets from an Ethernet device ( ssn_vnf_port::rx_burst() )
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
  { return rx->get_cons_perf(); }

  virtual size_t get_inner_tx_perf() const override
  { return tx->get_prod_perf(); }

  virtual size_t get_outer_rx_perf() const override
  { return rx->get_prod_perf(); }

  virtual size_t get_outer_tx_perf() const override
  { return tx->get_cons_perf(); }

  /**
   * @brief update port-statistics for timer-function.
   * @details
   *   This function must be called once a second.
   */
  virtual void stats_update_per1sec() override
  {
    rx->update_stats();
  }

}; /* class ssn_vnf_port_virt */



/**
 * @brief Provide 1 patch panel between a pair of ssn_vnf_port_virts
 * @details
 *   This class was designed for NFV/SFC (Service Function Chaining).
 *   This class connect a pair of ssn_vnf_port_virt classes.
 */
class ssn_vnf_port_patch_panel {

  ssn_vnf_port_virt* left;
  ssn_vnf_port_virt* right;

  ssn_ma_ring left_enq;
  ssn_ma_ring right_enq;

 public:

  /**
   * @brief constructor
   * @param [in] r right-side of port's pointer
   * @param [in] l left-side of port's pointer
   * @param [in] n_que number of tx and rx queues for RSS multiqueues
   */
  ssn_vnf_port_patch_panel(ssn_vnf_port_virt* r, ssn_vnf_port_virt* l, size_t n_que)
    : right(r), left(l), left_enq("slankdev"), right_enq("slankdedf")
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

    left_enq.configure_que(n_que);
    right_enq.configure_que(n_que);

    left->tx  = &left_enq;
    left->rx  = &right_enq;
    right->tx = &right_enq;
    right->rx = &left_enq;

    printf("patch panel create success\n");
  }

  /**
   * @brief destructor
   */
  virtual ~ssn_vnf_port_patch_panel() {}

}; /* class ssn_vnf_port_patch_panel */


/**
 * @brief Virtual lcore for VNFs
 */
class ssn_vnf_vcore {
 public:
  size_t lcore_id;
  slankdev::fixed_size_vector<size_t> port_rx_acc;
  slankdev::fixed_size_vector<size_t> port_tx_acc;
  ssn_vnf_vcore(size_t lcoreid, size_t n_rx_port, size_t n_tx_port)
    : lcore_id(lcoreid), port_rx_acc(n_rx_port), port_tx_acc(n_tx_port) {}
}; /* class ssn_vnf_vcore */


/**
 * @brief provide interface of VNF's block for implement VNF
 */
class ssn_vnf_block {
 private:
  static void _vnf_piece_spawner(void* instance_)
  {
    ssn_vnf_block* vi = reinterpret_cast<ssn_vnf_block*>(instance_);
    vi->deploy_impl(nullptr);
  }
  std::vector<uint32_t> tids;
  std::vector<ssn_vnf_vcore> vcores;
  slankdev::fixed_size_vector<ssn_vnf_port*>& ports;
  uint32_t coremask;

 protected:

  /**
   * @brief vnf implementation
   * @details
   *   User need to implement the behaviour of VNF-Block in this function.
   *   This function is called in ssn_vnf::deploy().
   */
  virtual void deploy_impl(void*) = 0;

  /**
   * @brief vnf implementation
   * @details
   *   User need to implement the behaviour to undeploy VNF-Block.
   *   This function is called in ssn_vnf::undeploy().
   */
  virtual void undeploy_impl() = 0;

  /**
   * @brief virtual function called in ssn_vnf::set_coremask()
   * @details
   *   If user want design new vnf class, It must call the access
   *   request funtions, such as ssn_vnf_block::request_rx_access()
   *   or ssn_vnf_block::request_tx_access().
   */
  virtual void set_coremask_impl(uint32_t coremask) = 0;

  /**
   * @brief Send a burst of output packets of an Ethernet device
   * @param [in] pid Port ID
   * @param [in] aid Access ID
   * @param [in] mbufs The address of an array of nb_pkts pointers
   *             to rte_mbuf structures which contain the output packets.
   * @param [in] n_mbufs The maximum number of packets to transmit.
   */
  size_t tx_burst(size_t pid, size_t aid, rte_mbuf** mbufs, size_t n_mbufs)
  { return ports[pid]->tx_burst(aid, mbufs, n_mbufs); }

  /**
   * @brief Receive a burst of output packets from an Ethernet device
   * @param [in] pid Port ID
   * @param [in] aid Access ID
   * @param [in] mbufs The address of an array of pointers to rte_mbuf
   *             structures that must be large enough to store nb_pkts
   *             pointers in it.
   * @param [in] n_mbufs The maximum number of packets to retrieve
   */
  size_t rx_burst(size_t pid, size_t aid, rte_mbuf** mbufs, size_t n_mbufs)
  { return ports[pid]->rx_burst(aid, mbufs, n_mbufs); }

  /**
   * @brief Request rx-access to port
   * @param pid port-id
   * @return rx-access-id for specilized-port
   */
  size_t port_request_rx_access(size_t pid)
  { return ports.at(pid)->request_rx_access(); }

  /**
   * @brief Request tx-access to port
   * @param pid port-id
   * @return tx-access-id for specilized-port
   */
  size_t port_request_tx_access(size_t pid)
  { return ports.at(pid)->request_tx_access(); }

  /**
   * @brief set rx access id of port from vcore-id
   * @param [in] vcore_id vcore-id in VNF Block
   * @param [in] pid port-id on VNF
   * @param [in] aid Specialized-port' rx-access-id for Specialized-vcore to set
   */
  void set_lcore_port_rxaid(size_t lcoreid, size_t pid, size_t aid)
  { vcores.at(lcoreid).port_rx_acc.at(pid) = aid; }

  /**
   * @brief set tx access id of port from vcore-id
   * @param [in] vcore_id vcore-id in VNF Block
   * @param [in] pid port-id on VNF
   * @param [in] aid Specialized-port' tx-access-id for Specialized-vcore to set
   */
  void set_lcore_port_txaid(size_t lcoreid, size_t pid, size_t aid)
  { vcores.at(lcoreid).port_tx_acc.at(pid) = aid; }

  /**
   * @brief get tx access id of port from vcore-id
   * @param [in] vcore_id vcore-id in VNF Block
   * @param [in] pid port-id on VNF
   * @return tx access-id of Specialized-port for Specialized-vcore
   */
  size_t get_lcore_port_rxaid(size_t vcore_id, size_t pid) const
  { return vcores.at(vcore_id).port_rx_acc.at(pid); }

  /**
   * @brief get rx access id of port from vcore-id
   * @param [in] vcore_id vcore-id in VNF Block
   * @param [in] pid port-id on VNF
   * @return rx access-id of Specialized-port for Specialized-vcore
   */
  size_t get_lcore_port_txaid(size_t vcore_id, size_t pid) const
  { return vcores.at(vcore_id).port_tx_acc.at(pid); }

  /**
   * @brief get lcore-id from vcore-id
   * @param [in] vcore_id vcore-id in VNF Block
   * @return logical core id (lcore-id)
   */
  size_t vcore_id_2_lcore_id(size_t vcore_id) const { return vcores.at(vcore_id).lcore_id; }

  /**
   * @brief get number of vcores on VNF
   * @return number of vcores on VNF
   */
  size_t n_vcores() const { return vcores.size(); }

  /**
   * @brief get number of ports on VNF
   * @return number of ports on VNF
   */
  size_t n_ports() const { return ports.size(); }

  /**
   * @brief get vlcore_id from running thread information
   * @return vlcore-id in VNF-Block
   */
  size_t get_vlcore_id() const
  {
    size_t lcore_id = ssn_lcore_id();
    for (size_t i=0; i<vcores.size(); i++) {
      if (vcores.at(i).lcore_id == lcore_id)
        return i;
    }
    std::string err = "vnf_impl::get_vlcore_id";
    err += slankdev::format("(%zd): ", lcore_id);
    err += "not found aid (lcoreid is invalid?)";
    throw slankdev::exception(err.c_str());
  }

 public:

  const std::string name;

  ssn_vnf_block(slankdev::fixed_size_vector<ssn_vnf_port*>& p, const char* n)
    : ports(p), name(n) {}

  virtual void debug_dump(FILE* fp) const = 0;

  virtual bool is_running() const = 0;

  void attach_port(size_t pid, ssn_vnf_port* p) { ports.at(pid) = p; }

  void set_coremask(uint32_t lcore_mask)
  {
    this->coremask = lcore_mask;
    vcores.clear();
    for (size_t i=0; i<32; i++) {
      if ((coremask & (0x1<<i)) != 0)
        vcores.push_back(ssn_vnf_vcore(i, ports.size(), ports.size()));
    }
    tids.resize(vcores.size());
    set_coremask_impl(coremask);
  }

  void deploy()
  {
    for (size_t i=0; i<vcores.size(); i++) {
      tids.at(i) = ssn_thread_launch(_vnf_piece_spawner, this, vcores.at(i).lcore_id);
    }
  }

  void undeploy()
  {
    assert(tids.size() == vcores.size());
    undeploy_impl();
    for (size_t i=0; i<vcores.size(); i++) {
      ssn_thread_join(tids.at(i));
    }
  }

  /**
   * @brief get coremask
   * @return coremask uint32_t
   */
  uint32_t get_coremask() const { return coremask; }

}; /* class ssn_vnf_block */


/**
 * @brief Provide VNF Interface.
 */
class ssn_vnf {
 private:

  /**
   * @brief Configuration port accessors
   * @details
   *   This function must be called after calling ssn_vnf::set_coremask()
   */
  void configre_acc()
  {
    size_t n_ports = ports.size();
    for (size_t i=0; i<n_ports; i++) {
      ports.at(i)->config_acc();
    }
  }

 protected:

  slankdev::fixed_size_vector<ssn_vnf_port*> ports;
  std::vector<ssn_vnf_block*> blocks;

 public:
  const std::string name;

  /**
   * @brief get number of ports
   * @return n_ports
   */
  size_t n_ports() const { return ports.size(); }

  /**
   * @brief get number of blocks
   * @return n_blocks
   */
  size_t n_blocks() const { return blocks.size(); }

  /**
   * @brief constructor
   * @param [in] nport number of ports included vnf.
   */
  ssn_vnf(size_t nport, const char* n) : ports(nport), name(n) {}

  /**
   * @brief reset all port's access config.
   * @details
   *   This function must be called before colling set_coremask().
   */
  void reset_allport_acc()
  {
    size_t n_ports = ports.size();
    for (size_t i=0; i<n_ports; i++) {
      ports.at(i)->reset_acc();
    }
  }

  /**
   * @brief set logical coremask to Block
   * @param [in] block_id block id
   * @param [in] cmask coremask
   * @details
   *   ex. If user want bind 2 lcores (lcore2 and lcore4),
   *   coremask is 0x14 (0b00010100)
   *   ex. lcore3 only: coremask is 0x08 (0x00001000)
   *   ex. lcore2 and lcore3: coremask is 0x0c (0x00001100)
   *   User must call this->reset_allport_acc() before calling this function.
   */
  void set_coremask(size_t block_id, uint32_t cmask)
  { blocks.at(block_id)->set_coremask(cmask); }

  /**
   * @brief attach vnf's port
   * @param [in] pid vnf's port id
   * @param [in] port port class's pointer to attach to vnf
   */
  void attach_port(size_t pid, ssn_vnf_port* port)
  {
    ports.at(pid) = port;
    auto n = blocks.size();
    for (size_t i=0; i<n; i++) {
      blocks.at(i)->attach_port(pid, port);
    }
  }

  /**
   * @brief get vnf's port as pointer to get stats
   * @param [in] pid port id
   * @return ssn_vnf_port pointer
   */
  const ssn_vnf_port* get_port(size_t pid) const
  { return ports.at(pid); }

  /**
   * @brief get vnf's block as pointer to get stats
   * @param [in] bid block id
   * @return ssn_vnf_block pointer
   */
  const ssn_vnf_block* get_block(size_t bid) const
  { return blocks.at(bid); }

  /**
   * @brief Debug output
   * @param fp FILE* file pointer to output
   */
  void debug_dump(FILE* fp) const
  {
    using std::string;
    fprintf(fp, "\r\n");
    fprintf(fp, "-[infos]-----------------------------------\r\n");
    fprintf(fp, " + name: \"%s\" \r\n", name.c_str());
    fprintf(fp, "-[block]-----------------------------------\r\n");
    auto n = blocks.size();
    for (size_t i=0; i<n; i++) {
      blocks.at(i)->debug_dump(fp);
    }
    fprintf(fp, "-[ports]-----------------------------------\r\n");
    n = ports.size();
    for (size_t i=0; i<n; i++) {
      auto* port = ports.at(i);
      string name = port->name;
      size_t orx = port->get_outer_rx_perf();
      size_t irx = port->get_inner_rx_perf();
      double r   = port->get_perf_reduction();
      printf("port[%zd]: name=%s orx=%zd irx=%zd red=%lf\n",
          i, name.c_str(), orx, irx, r);
    }
    fprintf(fp, "-------------------------------------------\r\n");
    fprintf(fp, "\r\n");
  }

  /**
   * @brief Deploy VNF
   * @details
   *   This function calles ssn_vnf::configure_acc() inner itselfs.
   */
  void deploy()
  {
    configre_acc();
    auto n_impl = blocks.size();
    for (size_t i=0; i<n_impl; i++) {
      this->blocks.at(i)->deploy();
    }
  }

  /**
   * @brief Deploy VNF
   */
  void undeploy()
  {
    auto n_impl = blocks.size();
    for (size_t i=0; i<n_impl; i++) {
      this->blocks.at(i)->undeploy();
    }
  }

  void update_stats()
  {
    auto np = ports.size();
    for (size_t i=0; i<np; i++) {
      this->ports.at(i)->stats_update_per1sec();
    }
  }

  bool is_running() const
  {
    auto nb = n_blocks();
    for (size_t i=0; i<nb; i++) {
      if (!blocks.at(i)->is_running()) return false;
    }
    return true;
  }

}; /* class ssn_vnf */




