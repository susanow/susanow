
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


class ssn_vnf_port_neo : public ssn_vnf_port {
 private:
  const size_t port_id;
  size_t n_rxacc;
  size_t n_txacc;

 public:
  ssn_vnf_port_neo(size_t pid, size_t nrxq, size_t ntxq)
    : ssn_vnf_port(pid, nrxq, ntxq), port_id(pid), n_rxacc(0), n_txacc(0) {}
  void reset_acc() { n_rxacc = 0; n_txacc = 0; }
  size_t request_rx_access()
  {
    auto tmp = n_rxacc;
    n_rxacc += 1;
    return tmp;
  }
  size_t request_tx_access()
  {
    auto tmp = n_txacc;
    n_txacc += 1;
    return tmp;
  }
  void config_acc() { this->configure(n_rxacc, n_txacc); }
};

class vnf_lcore {
 public:
  size_t lcore_id;
  std::vector<size_t> port_rx_acc;
  std::vector<size_t> port_tx_acc;
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
  uint32_t coremask;

 protected:
  virtual void deploy_impl(void*) = 0;
  virtual void undeploy_impl() = 0;
  virtual bool is_running() const = 0;
  virtual void set_coremask_impl(uint32_t coremask) = 0;
  virtual size_t n_rx_ports() const = 0;
  virtual size_t n_tx_ports() const = 0;

  size_t tx_burst(size_t pid, size_t aid, rte_mbuf** mbufs, size_t n_mbufs)
  { return ports[pid]->tx_burst(aid, mbufs, n_mbufs); }
  size_t rx_burst(size_t pid, size_t aid, rte_mbuf** mbufs, size_t n_mbufs)
  { return ports[pid]->rx_burst(aid, mbufs, n_mbufs); }

 public:
  vnf_impl(std::vector<ssn_vnf_port_neo*>& p) : ports(p) {}

  std::vector<vnf_lcore> lcores;
  std::vector<ssn_vnf_port_neo*>& ports;
  virtual void debug_dump(FILE* fp) const = 0;
  void attach_port(size_t pid, ssn_vnf_port_neo* p) { ports.at(pid) = p; }
  void set_coremask(uint32_t lcore_mask)
  {
    this->coremask = lcore_mask;

    lcores.clear();
    for (size_t i=0; i<32; i++) {
      if ((coremask & (0x1<<i)) != 0)
        lcores.push_back(vnf_lcore(i, n_rx_ports(), n_tx_ports()));
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
};


class vnf {
 protected:
  std::vector<ssn_vnf_port_neo*> ports;
  std::vector<vnf_impl*> impls;

 public:
  vnf(size_t nport) : ports(nport) {}
  void add_impl(vnf_impl* impl)
  {
    impls.push_back(impl);
    // printf("n_impls: %zd \n", impls.size());
  }
  void attach_port(size_t pid, ssn_vnf_port_neo* port)
  {
    ports.at(pid) = port;

    auto n = impls.size();
    for (size_t i=0; i<n; i++) {
      impls[i]->attach_port(pid, port);
    }
  }
  void set_coremask(size_t impl_id, uint32_t cmask)
  {
    impls.at(impl_id)->set_coremask(cmask);
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
};


