
#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <vector>

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


#include <ssn_port.h>
#include <ssn_port_stat.h>
#include <ssn_ma_port.h>
#include <ssn_log.h>
#include <ssn_cpu.h>
#include <ssn_common.h>
#include <ssn_thread.h>

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <vector>
#include <assert.h>
#include <slankdev/string.h>
#include <slankdev/exception.h>


ssn_vnf_port::ssn_vnf_port(size_t a_port_id, size_t a_n_rxq, size_t a_n_txq) :
  port_id(a_port_id), n_rxq(a_n_rxq), n_txq(a_n_txq), n_rxacc(0), n_txacc(0)
{
  ssn_ma_port_configure_hw(port_id, n_rxq, n_txq);
  ssn_ma_port_dev_up(port_id);
  ssn_ma_port_promisc_on(port_id);
}

void ssn_vnf_port::configure(size_t n_rxacc_, size_t n_txacc_)
{
  n_rxacc = n_rxacc_;
  n_txacc = n_txacc_;
  ssn_ma_port_configure_acc(port_id, n_rxacc, n_txacc);
}

size_t ssn_vnf_port::tx_burst(size_t aid, rte_mbuf** mbufs, size_t n_mbufs)
{ return ssn_ma_port_tx_burst(port_id, aid, mbufs, n_mbufs); }

size_t ssn_vnf_port::rx_burst(size_t aid, rte_mbuf** mbufs, size_t n_mbufs)
{ return ssn_ma_port_rx_burst(port_id, aid, mbufs, n_mbufs); }

void ssn_vnf_port::debug_dump(FILE* fp) const
{
  fprintf(fp, " port_id: %zd\r\n", port_id);
  fprintf(fp, " n_rxq  : %zd\r\n", n_rxq  );
  fprintf(fp, " n_txq  : %zd\r\n", n_txq  );
  fprintf(fp, " n_rxacc: %zd\r\n", n_rxacc);
  fprintf(fp, " n_txacc: %zd\r\n", n_txacc);
}


