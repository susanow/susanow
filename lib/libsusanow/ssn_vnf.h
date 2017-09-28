
/*
 * MIT License
 *
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
#include <vector>

struct rte_mbuf;

/**
 * @brief vnf port class
 * @details
 *   Now, this class implement for only dpdk-phy-port. but we are going to
 *   extend this class to support virtual-port for NF-chaining.
 */
class ssn_vnf_port {

  const size_t port_id; /*! dpdk port id */
  const size_t n_rxq;   /*! num of rx queues (hardware) */
  const size_t n_txq;   /*! num of tx queues (hardware) */
  size_t n_rxacc;       /*! num of rx accessor */
  size_t n_txacc;       /*! num of tx accessor */

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
  ssn_vnf_port(size_t a_port_id, size_t a_n_rxq, size_t a_n_txq);

  /**
   * @brief constructor
   * @param [in] n_rxacc num of accessor for rx only
   * @param [in] n_txacc num of accessor for tx only
   * @details
   *   This configuration can be changed while running dynamicaly
   *   for Susanow Dynamic Auto NF Optimization.
   *   This function calls ssn_ma_port_configure_acc internally
   */
  void configure(size_t n_rxacc_, size_t n_txacc_);

  /**
   * @brief Send a burst of output packets of an Ethernet device
   * @param [in] aid Access ID
   * @param [in] mbufs The address of an array of nb_pkts pointers
   *             to rte_mbuf structures which contain the output packets.
   * @param [in] n_mbufs The maximum number of packets to transmit.
   * @details
   *   User need to configure ssn_vnf_port instance with configure()
   *   before of calling this function.
   *   This function calls ssn_ma_port_tx_burst internally
   */
  size_t tx_burst(size_t aid, rte_mbuf** mbufs, size_t n_mbufs);


  /**
   * @brief Receive a burst of output packets from an Ethernet device
   * @param [in] aid Access ID
   * @param [in] mbufs The address of an array of pointers to rte_mbuf
   *             structures that must be large enough to store nb_pkts
   *             pointers in it.
   * @param [in] n_mbufs The maximum number of packets to retrieve
   * @details
   *   User need to configure ssn_vnf_port instance with configure()
   *   before of calling this function.
   *   This function calls ssn_ma_port_rx_burst internally
   */
  size_t rx_burst(size_t aid, rte_mbuf** mbufs, size_t n_mbufs);

  /**
   * @brief Debug output
   * @param fp FILE* file pointer to output
   */
  void debug_dump(FILE* fp) const;
};


/**
 * @brief VNF class
 */
class ssn_vnf {
  friend void _vnf_thread_spawner(void*);
 private:
  std::vector<ssn_vnf_port*> ports; /*! VNF's port vector                  */
  std::vector<uint32_t> tids;       /*! thread-ids for running-information */
  std::vector<size_t> lcores;       /*! lcore-ids assined                  */
  uint32_t coremask;                /*! running coremask                   */

 protected:
  virtual void deploy_imple(void* acc_id_) = 0;
  virtual void undeploy_imple() = 0;
  size_t tx_burst(size_t pid, size_t aid, rte_mbuf** mbufs, size_t n_mbufs);
  size_t rx_burst(size_t pid, size_t aid, rte_mbuf** mbufs, size_t n_mbufs);

 public:
  ssn_vnf(size_t n_ports_a);
  virtual ~ssn_vnf();
  virtual bool is_running() const = 0;
  size_t n_ports() const;
  void attach_port(size_t pid, ssn_vnf_port* p);
  void deploy(uint32_t lcore_mask);
  void undeploy();
  size_t get_aid_from_lcoreid(size_t lcore_id) const;
  size_t get_aid() const;
  void debug_dump(FILE* fp) const;
};


