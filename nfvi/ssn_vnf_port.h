
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
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string>


struct rte_mbuf;
class ssn_vnf;


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
   * @brief get device's socket-id
   * @return socket-id. 0,1,2...
   */
  virtual size_t get_socket_id() const = 0;

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
  size_t request_rx_access();

  /*
   * @brief Tx acccess request
   * @return Tx Access ID
   * @details
   *   Accessor must call this function and this function return.
   *   access-id. Accessor can tx-access via this access-id.
   */
  size_t request_tx_access();

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
   *     2: inner tx perf (==4)  : can get tx_burst call-info
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
  double get_perf_reduction() const;

  /**
   * @brief update vnf_port's status info
   * @details
   *   this function must be call per 1 second
   *   by Hypervisor.
   */
  virtual void stats_update_per1sec() = 0;

}; /* class ssn_vnf_port */



