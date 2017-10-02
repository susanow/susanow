
#pragma once
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <slankdev/string.h>
#include <slankdev/exception.h>
#include <slankdev/util.h>
#include <exception>

#include <ssn_thread.h>
#include <ssn_cpu.h>
#include <ssn_port.h>
#include <ssn_common.h>
#include <ssn_log.h>
#include <dpdk/dpdk.h>
#include <vector>


template <class T>
class fixed_size_vector {
  std::vector<T> _vec;
 public:
  fixed_size_vector(size_t n) : _vec(n) {}
  T& at(size_t ix) { return _vec.at(ix); }
  const T& at(size_t ix) const { return _vec.at(ix); }
  size_t size() const { return _vec.size(); }
  T& operator[](size_t ix) { return _vec[ix]; }
  const T& operator[](size_t ix) const { return _vec[ix]; }
};


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
  size_t tx_burst(size_t aid, rte_mbuf** mbufs, size_t n_mbufs);

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
  size_t rx_burst(size_t aid, rte_mbuf** mbufs, size_t n_mbufs);

  /**
   * @brief Debug output
   * @param fp FILE* file pointer to output
   */
  void debug_dump(FILE* fp) const;

  /**
   * @brief Configure form accessors
   * @details
   *   This configuration can be changed while running dynamicaly
   *   for Susanow Dynamic Auto NF Optimization.
   *   This function calls ssn_ma_port_configure_acc internally
   */
  void config_acc()
  { ssn_ma_port_configure_acc(port_id, n_rxacc, n_txacc); }

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

  void reset_acc() { n_rxacc = 0; n_txacc = 0; }
  size_t get_n_rxq() const { return n_rxq; }
  size_t get_n_txq() const { return n_txq; }
  size_t get_n_rxacc() const { return n_rxacc; }
  size_t get_n_txacc() const { return n_txacc; }
}; /* class ssn_vnf_port */


class ssn_vnf_vcore {
 public:
  size_t lcore_id;
  fixed_size_vector<size_t> port_rx_acc;
  fixed_size_vector<size_t> port_tx_acc;
  ssn_vnf_vcore(size_t lcoreid, size_t n_rx_port, size_t n_tx_port)
    : lcore_id(lcoreid), port_rx_acc(n_rx_port), port_tx_acc(n_tx_port) {}
};

class vnf_impl {
 private:
  static void _vnf_imple_spawner(void* instance_)
  {
    vnf_impl* vi = reinterpret_cast<vnf_impl*>(instance_);
    vi->deploy_impl(nullptr);
  }
  std::vector<uint32_t> tids;
  std::vector<ssn_vnf_vcore> lcores;
  fixed_size_vector<ssn_vnf_port*>& ports;
  uint32_t coremask;

 protected:
  virtual void deploy_impl(void*) = 0;
  virtual void undeploy_impl() = 0;
  virtual void set_coremask_impl(uint32_t coremask) = 0;

  size_t tx_burst(size_t pid, size_t aid, rte_mbuf** mbufs, size_t n_mbufs)
  { return ports[pid]->tx_burst(aid, mbufs, n_mbufs); }
  size_t rx_burst(size_t pid, size_t aid, rte_mbuf** mbufs, size_t n_mbufs)
  { return ports[pid]->rx_burst(aid, mbufs, n_mbufs); }

  size_t port_request_rx_access(size_t pid)
  { return ports.at(pid)->request_rx_access(); }
  size_t port_request_tx_access(size_t pid)
  { return ports.at(pid)->request_tx_access(); }

  void set_lcore_port_rxaid(size_t lcoreid, size_t pid, size_t aid)
  { lcores.at(lcoreid).port_rx_acc.at(pid) = aid; }
  void set_lcore_port_txaid(size_t lcoreid, size_t pid, size_t aid)
  { lcores.at(lcoreid).port_tx_acc.at(pid) = aid; }

  size_t get_lcore_port_rxaid(size_t vcore_id, size_t pid) const
  { return lcores.at(vcore_id).port_rx_acc.at(pid); }
  size_t get_lcore_port_txaid(size_t vcore_id, size_t pid) const
  { return lcores.at(vcore_id).port_tx_acc.at(pid); }

  size_t vcore_id_2_lcore_id(size_t vcore_id) const { return lcores[vcore_id].lcore_id; }
  size_t n_vcores() const { return lcores.size(); }
  size_t n_ports() const { return ports.size(); }
  size_t get_vlcore_id() const
  {
    size_t lcore_id = ssn_lcore_id();
    for (size_t i=0; i<lcores.size(); i++) {
      if (lcores.at(i).lcore_id == lcore_id)
        return i;
    }
    std::string err = "vnf_impl::get_vlcore_id";
    err += slankdev::format("(%zd): ", lcore_id);
    err += "not found aid (lcoreid is invalid?)";
    throw slankdev::exception(err.c_str());
  }

 public:
  vnf_impl(fixed_size_vector<ssn_vnf_port*>& p) : ports(p) {}
  virtual void debug_dump(FILE* fp) const = 0;
  virtual bool is_running() const = 0;

  void attach_port(size_t pid, ssn_vnf_port* p) { ports.at(pid) = p; }
  void set_coremask(uint32_t lcore_mask)
  {
    this->coremask = lcore_mask;
    lcores.clear();
    for (size_t i=0; i<32; i++) {
      if ((coremask & (0x1<<i)) != 0)
        lcores.push_back(ssn_vnf_vcore(i, ports.size(), ports.size()));
    }
    tids.resize(lcores.size());
    set_coremask_impl(coremask);
  }
  void deploy()
  {
    for (size_t i=0; i<lcores.size(); i++) {
      tids.at(i) = ssn_thread_launch(_vnf_imple_spawner, this, lcores[i].lcore_id);
    }
  }
  void undeploy()
  {
    assert(tids.size() == lcores.size());
    undeploy_impl();
    for (size_t i=0; i<lcores.size(); i++) {
      ssn_thread_join(tids.at(i));
    }
  }
}; /* class vnf_impl */


class vnf {
 protected:
  fixed_size_vector<ssn_vnf_port*> ports;
  std::vector<vnf_impl*> impls;

 public:
  vnf(size_t nport) : ports(nport) {}

  void add_impl(vnf_impl* impl) { impls.push_back(impl); }

  void set_coremask(size_t impl_id, uint32_t cmask)
  { impls.at(impl_id)->set_coremask(cmask); }

  void attach_port(size_t pid, ssn_vnf_port* port)
  {
    ports.at(pid) = port;
    auto n = impls.size();
    for (size_t i=0; i<n; i++) {
      impls[i]->attach_port(pid, port);
    }
  }

  void config_port_acc()
  {
    size_t n_ports = ports.size();
    for (size_t i=0; i<n_ports; i++) {
      ports.at(i)->config_acc();
    }
  }

  void debug_dump(FILE* fp) const
  {
    fprintf(fp, "\r\n");
    auto n = impls.size();
    for (size_t i=0; i<n; i++) {
      impls.at(i)->debug_dump(fp);
    }
    fprintf(fp, "\r\n");
  }

  void deploy()
  {
    auto n_impl = impls.size();
    for (size_t i=0; i<n_impl; i++) {
      this->impls.at(i)->deploy();
    }
  }

  void undeploy()
  {
    auto n_impl = impls.size();
    for (size_t i=0; i<n_impl; i++) {
      this->impls.at(i)->undeploy();
    }
  }
}; /* class vnf */




/*****************************************************************************\
 * Bellow are Implementation
\*****************************************************************************/

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


