
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

#include <ssn_thread.h>
#include <ssn_cpu.h>
#include <ssn_port.h>
#include <ssn_common.h>
#include <ssn_log.h>
#include <ssn_vnf.h>
#include <dpdk/dpdk.h>


ssn_vnf_port* port[2];

struct access_ids {
  size_t rx_aid;
  size_t tx_aid;
};
access_ids aids[RTE_MAX_LCORE];

size_t get_oportid_from_iportid(size_t in_port_id) { return in_port_id^1; }

class vnf_impl {
 public:
  uint32_t coremask;
  size_t rx_aid;
  size_t tx_aid;
  void configure(uint32_t coremask_)
  {
    coremask = coremask_;
  }
  virtual void deploy_impl(void*) = 0;
  virtual void undeploy_impl() = 0;
};
class vnf_impl_port0 : public vnf_impl {
 public:
  const size_t n_ports = 2;
  bool running;
  virtual void undeploy_impl() override { running = false; }
  virtual void deploy_impl(void*) override
  {
    constexpr size_t iport_id = 0;
    size_t rxaid = this->rx_aid;
    size_t txaid = this->tx_aid;
    printf("running impl_port0\n");
    running = true;
    while (running) {
      rte_mbuf* mbufs[32];
      size_t n_recv = port[iport_id]->rx_burst(rxaid, mbufs, 32);
      if (n_recv == 0) continue;

      for (size_t i=0; i<n_recv; i++) {

        /* Delay Block begin */
        size_t n=10;
        for (size_t j=0; j<100; j++) n++;

        size_t oport_id = get_oportid_from_iportid(iport_id);
        port[oport_id]->tx_burst(txaid, &mbufs[i], 1);
      }
    }
    printf("finnish impl_port0\n");
  }
};
class vnf_impl_port1 : public vnf_impl {
 public:
  const size_t n_ports = 2;
  bool running;
  virtual void undeploy_impl() override { running = false; }
  virtual void deploy_impl(void*) override
  {
    constexpr size_t iport_id = 1;
    size_t rxaid = this->rx_aid;
    size_t txaid = this->tx_aid;
    printf("running impl_port1\n");
    running = true;
    while (running) {
      rte_mbuf* mbufs[32];
      size_t n_recv = port[iport_id]->rx_burst(rxaid, mbufs, 32);
      if (n_recv == 0) continue;

      for (size_t i=0; i<n_recv; i++) {

        /* Delay Block begin */
        size_t n=10;
        for (size_t j=0; j<100; j++) n++;

        size_t oport_id = get_oportid_from_iportid(iport_id);
        port[oport_id]->tx_burst(txaid, &mbufs[i], 1);
      }
    }
    printf("finnish impl_port1\n");
  }
};
void _vnf_imple_spawner(void* instance_)
{
  vnf_impl* vi = reinterpret_cast<vnf_impl*>(instance_);
  vi->deploy_impl(nullptr);
}
class vnf {
 protected:
  std::vector<size_t> tids;
  std::vector<vnf_impl*> impls;
  ssn_vnf_port* ports[2];
 public:
  vnf()
  {
    vnf_impl* vnf_impl0 = new vnf_impl_port0;
    vnf_impl* vnf_impl1 = new vnf_impl_port1;
    vnf_impl0->rx_aid = 0; vnf_impl0->tx_aid = 0;
    vnf_impl1->rx_aid = 0; vnf_impl1->tx_aid = 1;
    impls.push_back(vnf_impl0);
    impls.push_back(vnf_impl1);
  }
  void deploy(size_t coremask0, size_t coremask1)
  {
    tids.resize(2);
    assert(impls.size() == 2);
    assert(tids.size() == 2);

    size_t n_impls = impls.size();
    for (size_t i=0; i<n_impls; i++) {
      tids[i] = ssn_thread_launch(_vnf_imple_spawner, impls[i], i+1);
    }
  }
  void undeploy()
  {
    assert(impls.size() == tids.size());
    size_t n_impls = impls.size();
    for (size_t i=0; i<n_impls; i++) {
      impls[i]->undeploy_impl();
    }
    size_t n_tids = tids.size();
    for (size_t i=0; i<n_tids; i++) {
      ssn_thread_join(tids[i]);
    }
  }
  void attach_port(size_t pid, ssn_vnf_port* port)
  {
    if (pid >= 2)
      throw slankdev::exception("invalid port_id");
    ports[pid] = port;
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
  port[0] = new ssn_vnf_port(0, 2, 4); // dpdk0
  port[1] = new ssn_vnf_port(1, 2, 4); // dpdk1
  printf("\n");
  port[0]->debug_dump(stdout); printf("\n");
  port[1]->debug_dump(stdout); printf("\n");

  /*--------deploy-field-begin----------------------------------------------*/
  uint32_t tid0,tid1,tid2,tid3;

#if 0
  /* configuration 1 */
  port[0]->configure(1, 2);
  port[1]->configure(1, 2);
  vnf_impl* vnf_impl0 = new vnf_impl_port0;
  vnf_impl* vnf_impl1 = new vnf_impl_port1;
  vnf_impl0->rx_aid = 0; vnf_impl0->tx_aid = 0;
  vnf_impl1->rx_aid = 0; vnf_impl1->tx_aid = 1;
  tid0 = ssn_thread_launch(_vnf_imple_spawner, vnf_impl0, 1);
  tid1 = ssn_thread_launch(_vnf_imple_spawner, vnf_impl1, 2);
  getchar();
  vnf_impl0->undeploy_impl();
  vnf_impl1->undeploy_impl();
  ssn_thread_join(tid0);
  ssn_thread_join(tid1);
  delete vnf_impl0;
  delete vnf_impl1;

  /* configuration 2 */
  port[0]->configure(2, 4);
  port[1]->configure(2, 4);
  vnf_impl* vnf_impl0_0 = new vnf_impl_port0;
  vnf_impl* vnf_impl0_1 = new vnf_impl_port0;
  vnf_impl* vnf_impl1_0 = new vnf_impl_port1;
  vnf_impl* vnf_impl1_1 = new vnf_impl_port1;
  vnf_impl0_0->rx_aid = 0; vnf_impl0_0->tx_aid = 0;
  vnf_impl0_1->rx_aid = 1; vnf_impl0_1->tx_aid = 1;
  vnf_impl1_0->rx_aid = 0; vnf_impl1_0->tx_aid = 2;
  vnf_impl1_1->rx_aid = 1; vnf_impl1_1->tx_aid = 3;
  tid0 = ssn_thread_launch(_vnf_imple_spawner, vnf_impl0_0, 1);
  tid1 = ssn_thread_launch(_vnf_imple_spawner, vnf_impl0_1, 2);
  tid2 = ssn_thread_launch(_vnf_imple_spawner, vnf_impl1_0, 3);
  tid3 = ssn_thread_launch(_vnf_imple_spawner, vnf_impl1_1, 4);
  getchar();
  vnf_impl0_0->undeploy_impl();
  vnf_impl0_1->undeploy_impl();
  vnf_impl1_0->undeploy_impl();
  vnf_impl1_1->undeploy_impl();
  ssn_thread_join(tid0);
  ssn_thread_join(tid1);
  ssn_thread_join(tid2);
  ssn_thread_join(tid3);
  delete vnf_impl0_0;
  delete vnf_impl0_1;
  delete vnf_impl1_0;
  delete vnf_impl1_1;
#else
  vnf v0;
  v0.attach_port(0, port[0]);
  v0.attach_port(1, port[1]);

  /* configuration 2 */
  port[0]->configure(1, 2);
  port[1]->configure(1, 2);
  v0.deploy(0x02, 0x04);
  // v0.impl[0].deploy(0x02);
  // v0.impl[1].deploy(0x04);
  getchar();
  v0.undeploy();

  // #<{(| configuration 2 |)}>#
  // port[0]->configure(1, 2);
  // port[1]->configure(1, 2);
  // v0.impl[0].deploy(0x06);
  // v0.impl[1].deploy(0x24);
  // getchar();
  // v0.undeploy();
#endif

  /*--------deploy-field-end------------------------------------------------*/

  delete port[0];
  delete port[1];
  ssn_fin();
}


