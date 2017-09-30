
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

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <slankdev/string.h>
#include <slankdev/exception.h>
#include <exception>

#include <ssn_thread.h>
#include <ssn_cpu.h>
#include <ssn_port.h>
#include <ssn_common.h>
#include <ssn_log.h>
#include <ssn_vnf.h>
#include <dpdk/dpdk.h>

#define NOTIMPL(str) slankdev::exception("NOT IMPLEMENT " #str)

size_t popcnt(uint32_t n)
{
  size_t cnt = 0;
  for (size_t i=0; i<32; i++) {
    if ((0x01&n) != 0) cnt ++;
    n >>= 1;
  }
  return cnt;
}

size_t get_oportid_from_iportid(size_t in_port_id) { return in_port_id^1; }

class ssn_vnf_port_neo : public ssn_vnf_port {
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
  void config_acc()
  {
    printf("SSN_VNF_PORT_CONF(pid:%zd, n_rxac:%zd, n_txacc:%zd)\n", port_id, n_rxacc, n_txacc);
    this->configure(n_rxacc, n_txacc);
  }
};

struct vnf_lcore {
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

 public:
  std::vector<vnf_lcore> lcores;

 protected:
  virtual void deploy_impl(void*) = 0;
  virtual void undeploy_impl() = 0;
  virtual void set_coremask_impl(uint32_t coremask) = 0;
  virtual size_t n_rx_ports() const = 0;
  virtual size_t n_tx_ports() const = 0;

  size_t tx_burst(size_t pid, size_t aid, rte_mbuf** mbufs, size_t n_mbufs)
  { return ports[pid]->tx_burst(aid, mbufs, n_mbufs); }
  size_t rx_burst(size_t pid, size_t aid, rte_mbuf** mbufs, size_t n_mbufs)
  { return ports[pid]->rx_burst(aid, mbufs, n_mbufs); }

 public:
  std::vector<ssn_vnf_port_neo*> ports;
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
      tids[i] = ssn_thread_launch(_vnf_imple_spawner, this, lcores[i].lcore_id);
    }
  }
  void undeploy()
  {
    assert(tids.size() == lcores.size());

    undeploy_impl();
    for (size_t i=0; i<lcores.size(); i++) {
      ssn_thread_join(tids[i]);
    }
    printf("undeploy succes\n");
  }

  size_t get_vlcore_id() const
  {
    size_t lcore_id = ssn_lcore_id();
    for (size_t i=0; i<lcores.size(); i++) {
      if (lcores[i].lcore_id == lcore_id)
        return i;
    }
    std::string err = "vnf_impl::get_vlcore_id";
    err += slankdev::format("(%zd): ", lcore_id);
    err += "not found aid (lcoreid is invalid?)";
    throw slankdev::exception(err.c_str());
  }

  /* To Be Implement */
  // virtual bool is_running();
  // void debug_dump(FILE* fp) const;
};
class vnf_impl_port : public vnf_impl {
 public:
  const size_t port_id;
  const size_t n_ports = 2;
  bool running;

  vnf_impl_port(size_t polling_port_id) : port_id(polling_port_id)
  {
    ports.resize(2);
  }
  virtual size_t n_rx_ports() const override { return 2; }
  virtual size_t n_tx_ports() const override { return 2; }
  virtual void set_coremask_impl(uint32_t coremask) override
  {
    size_t n_lcores = popcnt(coremask);
    for (size_t i=0; i<n_lcores; i++) {
      size_t acc;

      acc = ports.at(port_id)->request_rx_access();
      lcores.at(i).port_rx_acc.at(port_id) = acc;

      acc = ports[port_id]->request_tx_access();
      lcores.at(i).port_tx_acc.at(port_id) = acc;

      acc = ports[port_id^1]->request_tx_access();
      lcores.at(i).port_tx_acc.at(port_id^1) = acc;
    }
  }
  virtual void undeploy_impl() override { running = false; }
  virtual void deploy_impl(void*) override
  {
    size_t plid = ssn_lcore_id();
    size_t vlid  = get_vlcore_id();
    printf("running impl_port vlcore%zd plcore%zd  \n", vlid, plid);
    running = true;
    // printf("BLOCKING with getcbar() for debug...\n"); getchar(); // TODO
    while (running) {
      rte_mbuf* mbufs[32];
      size_t rxaid = lcores[vlid].port_rx_acc[port_id];
      size_t n_recv = ports[port_id]->rx_burst(rxaid, mbufs, 32);
      if (n_recv == 0) continue;

      for (size_t i=0; i<n_recv; i++) {

        /* Delay Block begin */
        size_t n=10;
        for (size_t j=0; j<100; j++) n++;

        size_t oport_id = get_oportid_from_iportid(port_id);
        size_t txaid = lcores[vlid].port_tx_acc[oport_id];
        ports[oport_id]->tx_burst(txaid, &mbufs[i], 1);
      }
    }
    printf("finnish impl_port1\n");
  }
};
class vnf {
 public:
  std::vector<vnf_impl*> impls;
  vnf()
  {
    vnf_impl* vnf_impl0 = new vnf_impl_port(0);
    vnf_impl* vnf_impl1 = new vnf_impl_port(1);
    impls.push_back(vnf_impl0);
    impls.push_back(vnf_impl1);
  }
};


/*---------------------------------------------------------------------------*/

char waitmsg(const char* msg)
{
  printf(msg);
  return getchar();
}

int main(int argc, char** argv)
{
  ssn_init(argc, argv);
  size_t n_ports = ssn_dev_count();
  if (n_ports != 2) {
    std::string err = slankdev::format("n_ports is not 2 (current %zd)",
        ssn_dev_count());
    throw slankdev::exception(err.c_str());
  }

  printf("\n");
  ssn_vnf_port_neo* port[2];
  port[0] = new ssn_vnf_port_neo(0, 2, 4); // dpdk0
  port[1] = new ssn_vnf_port_neo(1, 2, 4); // dpdk1
  printf("\n");
  port[0]->debug_dump(stdout); printf("\n");
  port[1]->debug_dump(stdout); printf("\n");

  /*--------deploy-field-begin----------------------------------------------*/

  vnf v0;
  v0.impls[0]->attach_port(0, port[0]);
  v0.impls[0]->attach_port(1, port[1]);
  v0.impls[1]->attach_port(0, port[0]);
  v0.impls[1]->attach_port(1, port[1]);

  /* configuration 2 */
  v0.impls[0]->set_coremask(0x02);
  v0.impls[1]->set_coremask(0x04);
  port[0]->config_acc();
  port[1]->config_acc();
  v0.impls[0]->deploy();
  v0.impls[1]->deploy();
  getchar();
  v0.impls[0]->undeploy();
  v0.impls[1]->undeploy();

  /* Reset */
  port[0]->reset_acc(); port[1]->reset_acc();

  /* configuration 2 */
  v0.impls[0]->set_coremask(0x06);
  v0.impls[1]->set_coremask(0x18);
  port[0]->config_acc();
  port[1]->config_acc();
  v0.impls[0]->deploy();
  v0.impls[1]->deploy();
  getchar();
  v0.impls[0]->undeploy();
  v0.impls[1]->undeploy();


  /*--------deploy-field-end------------------------------------------------*/

  delete port[0];
  delete port[1];
  ssn_fin();
}


