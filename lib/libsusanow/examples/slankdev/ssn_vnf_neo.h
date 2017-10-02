
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
#include <ssn_vnf.h>
#include <dpdk/dpdk.h>


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


class ssn_vnf_port_neo : public ssn_vnf_port {
 private:
  const size_t port_id;
  size_t n_rxaid;
  size_t n_txaid;

 public:
  ssn_vnf_port_neo(size_t pid, size_t n_rxq, size_t n_txq)
    : ssn_vnf_port(pid, n_rxq, n_txq), port_id(pid), n_rxaid(0), n_txaid(0) {}
  void reset_acc() { n_rxaid = 0; n_txaid = 0; }
  size_t request_rx_access()
  {
    auto tmp = n_rxaid;
    n_rxaid += 1;
    return tmp;
  }
  size_t request_tx_access()
  {
    auto tmp = n_txaid;
    n_txaid += 1;
    return tmp;
  }
  void config_acc() { this->configure(n_rxaid, n_txaid); }
};

class vnf_lcore {
 public:
  size_t lcore_id;
  fixed_size_vector<size_t> port_rx_acc;
  fixed_size_vector<size_t> port_tx_acc;
  vnf_lcore(size_t lcoreid, size_t n_rx_port, size_t n_tx_port)
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
  std::vector<vnf_lcore> lcores;
  fixed_size_vector<ssn_vnf_port_neo*>& ports;
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

  size_t n_ports() const { return ports.size(); }
  size_t vcore_id_2_lcore_id(size_t vcore_id) const { return lcores[vcore_id].lcore_id; }
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
  vnf_impl(fixed_size_vector<ssn_vnf_port_neo*>& p) : ports(p) {}
  virtual void debug_dump(FILE* fp) const = 0;
  virtual bool is_running() const = 0;

  void attach_port(size_t pid, ssn_vnf_port_neo* p) { ports.at(pid) = p; }
  void set_coremask(uint32_t lcore_mask)
  {
    this->coremask = lcore_mask;
    lcores.clear();
    for (size_t i=0; i<32; i++) {
      if ((coremask & (0x1<<i)) != 0)
        lcores.push_back(vnf_lcore(i, ports.size(), ports.size()));
    }
    tids.resize(lcores.size());
    set_coremask_impl(coremask);
  }
  size_t n_vcores() const { return lcores.size(); }
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
  fixed_size_vector<ssn_vnf_port_neo*> ports;
  std::vector<vnf_impl*> impls;

 public:
  vnf(size_t nport) : ports(nport) {}

  void add_impl(vnf_impl* impl) { impls.push_back(impl); }

  void set_coremask(size_t impl_id, uint32_t cmask)
  { impls.at(impl_id)->set_coremask(cmask); }

  void attach_port(size_t pid, ssn_vnf_port_neo* port)
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
}; /* clas vnf */


