
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

  /**
   * @brief get device's socket-id
   * @return socket-id. 0,1,2...
   */
  virtual size_t get_socket_id() const override
  {
    if (tx && rx) {
      size_t tx_sockid = tx->get_socket_id();
      size_t rx_sockid = rx->get_socket_id();
      assert(tx_sockid == rx_sockid);
      return tx_sockid;
    } else {
      return 0;
    }
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



