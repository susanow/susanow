
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
#include <ssn_vnf_port.h>
#include <ssn_vnf_vcore.h>



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

  std::vector<size_t> socket_affinity; // This value is tmp value for sock-affinity
  size_t _socket_affinity;

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
  {
    auto* port  = ports.at(pid);
    size_t sock_id = port->get_socket_id();
    this->socket_affinity[sock_id]++;
    return port->request_rx_access();
  }

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
    : ports(p), name(n)
  {
    size_t n_socket = ssn_socket_count();
    socket_affinity.resize(n_socket);
  }

  virtual void debug_dump(FILE* fp) const = 0;

  virtual bool is_running() const = 0;

  void attach_port(size_t pid, ssn_vnf_port* p) { ports.at(pid) = p; }
  void dettach_port(size_t pid) { ports.at(pid) = nullptr; }

  /**
   * @brief get socket-id affinitied
   * @return socket-id should be deploy to
   * @details
   *   none
   */
  size_t get_socket_affinity() const { return _socket_affinity; }

  size_t get_socket_affinity_impl() const
  {
    size_t sid = 0;
    size_t max = socket_affinity[sid];
    const size_t n_sock = ssn_socket_count();
    for (size_t i=0; i<n_sock; i++) {
      if (socket_affinity[i] > max) {
        sid = i;
        max = socket_affinity[i];
      }
    }
    return sid;
  }

  /**
   * @brief reset block's running config
   * @details
   *   this function sets value for get_socket_affinity()
   */
  void reset()
  {
    set_coremask(0x1);
    _socket_affinity = get_socket_affinity_impl();

    const size_t n_ele = socket_affinity.size();
    for (size_t i=0; i<n_ele; i++) {
      socket_affinity[i] = 0;
    }
  }

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

  int deploy()
  {
    if (vcores.size() == 0) return -1;

    for (size_t i=0; i<vcores.size(); i++) {
      tids.at(i) = ssn_thread_launch(_vnf_piece_spawner, this, vcores.at(i).lcore_id);
    }
    return 0;
  }

  void undeploy()
  {
    assert(tids.size() == vcores.size());
    if (is_running()) {
      undeploy_impl();
      for (size_t i=0; i<vcores.size(); i++) {
        if (tids.at(i) == 0) {
          printf("okashii kedo return suru\n");
          return;
        }
        ssn_thread_join(tids.at(i));
      }
    } else {
      return;
    }
  }

  /**
   * @brief get coremask
   * @return coremask uint32_t
   */
  uint32_t get_coremask() const { return coremask; }

}; /* class ssn_vnf_block */




