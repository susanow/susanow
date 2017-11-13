
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
  void dettach_port(size_t pid) { ports.at(pid) = nullptr; }

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
  ssn_vnf(size_t nport, const char* n) : ports(nport), name(n)
  {
    const size_t n_ele = ports.size();
    for (size_t i=0; i<n_ele; i++) {
      ports.at(i) = nullptr;
    }
  }

  /**
   * @brief reset vnf's running config.
   * @return 0 success
   * @return -1 unsuccess, maybe port is not attached
   * @details
   *   Calling this functions needs that vnf already attached all ports.
   *   This function doesn't detach ports.
   *   This function must be called before colling set_coremask().
   */
  int reset()
  {
    /*
     * Reset coremask
     */
    const size_t n_ele = blocks.size();
    for (size_t i=0; i<n_ele; i++) {
      set_coremask(i, 0x00);
    }

    /*
     * Reset all port accessor
     */
    const size_t n_port = ports.size();
    for (size_t i=0; i<n_port; i++) {
      if (!ports.at(i)) return -1;
    }
    for (size_t i=0; i<n_port; i++) {
      ports.at(i)->reset_acc();
    }
    return 0;
  }

  /**
   * @brief set logical coremask to Block
   * @param [in] block_id block id
   * @param [in] cmask coremask
   * @return 0 success
   * @return -1 unsuccess, maybe port is not attached
   * @details
   *   Calling this functions needs that vnf already attached all ports.
   *   ex. If user want bind 2 lcores (lcore2 and lcore4),
   *   coremask is 0x14 (0b00010100)
   *   ex. lcore3 only: coremask is 0x08 (0x00001000)
   *   ex. lcore2 and lcore3: coremask is 0x0c (0x00001100)
   *   User must call this->reset() before calling this function.
   */
  int set_coremask(size_t block_id, uint32_t cmask)
  {
    const size_t n_port = ports.size();
    for (size_t i=0; i<n_port; i++) {
      if (!ports.at(i)) return -1;
    }
    blocks.at(block_id)->set_coremask(cmask);
    return 0;
  }

  /**
   * @brief check vnf is deletable
   * @return true deletable
   * @return false undeletable
   */
  bool is_deletable() const
  {
    const size_t n_ele = ports.size();
    for (size_t i=0; i<n_ele; i++) {
      if (ports.at(i) != nullptr) {
        return false;
      }
    }
    return true;
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
    fprintf(fp, " + name: \"%s\" \r\n", name.c_str());
    fprintf(fp, " + ports\r\n");
    size_t n = ports.size();
    for (size_t i=0; i<n; i++) {
      auto* port = ports.at(i);
      if (port) {
        string name = port->name;
        size_t orx = port->get_outer_rx_perf();
        size_t irx = port->get_inner_rx_perf();
        double r   = port->get_perf_reduction();
        printf("  port[%zd]: name=%s orx=%zd irx=%zd red=%lf\n",
            i, name.c_str(), orx, irx, r);
      } else {
        printf("  port[%zd]: null\n", i);
      }
    }
    fprintf(fp, "\r\n");
  }

  /**
   * @brief Deploy VNF
   * @return 0 success
   * @return -1 unsuccess, some-error occured
   * @details
   *   Calling this functions needs that vnf already attached all ports.
   *   This function calles ssn_vnf::configure_acc() inner itselfs.
   */
  int deploy()
  {
    const size_t n_port = ports.size();
    for (size_t i=0; i<n_port; i++) {
      if (!ports.at(i)) return -1;
    }
    configre_acc();
    auto n_impl = blocks.size();
    for (size_t i=0; i<n_impl; i++) {
      int ret = this->blocks.at(i)->deploy();
      if (ret < 0) {
        return -1;
      }
    }
    return 0;
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

  /**
   * @brief attach vnf's port
   * @param [in] pid vnf's port id
   * @param [in] port port class's pointer to attach to vnf
   * @return 0 success
   * @return -1 invalid port or already attached
   */
  int attach_port(size_t pid, ssn_vnf_port* port)
  {
    if (ports.at(pid) || !port) {
      /* port already attached */
      return -1;
    }
    ports.at(pid) = port;
    port->attach_vnf(this);
    auto n = blocks.size();
    for (size_t i=0; i<n; i++) {
      blocks.at(i)->attach_port(pid, port);
    }
    return 0;
  }

  /**
   * @brief dettach vnf's port
   * @param [in] pid vnf's port id
   * @return 0 success
   * @return -1 port does not attached
   */
  int dettach_port(size_t pid)
  {
    if (!ports.at(pid)) {
      /* unattached port yet */
      return -1;
    }
    ports.at(pid)->dettach_vnf();
    ports.at(pid) = nullptr;
    auto n = blocks.size();
    for (size_t i=0; i<n; i++) {
      blocks.at(i)->dettach_port(pid);
    }
    return 0;
  }

}; /* class ssn_vnf */




