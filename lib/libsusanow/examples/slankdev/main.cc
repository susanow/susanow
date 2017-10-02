
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

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <ssn_ma_port.h>
#include <ssn_vnf.h>
#define NOTIMPL(str) slankdev::exception("NOT IMPLEMENT " #str)

size_t get_oportid_from_iportid(size_t in_port_id) { return in_port_id^1; }

class vnf_impl_port : public vnf_impl {
 public:
  bool running = false;
  const size_t port_id;
  const std::string name;

  vnf_impl_port(size_t polling_port_id, const char* n,
      fixed_size_vector<ssn_vnf_port*>& ports)
    : vnf_impl(ports), port_id(polling_port_id), name(n) {}
  virtual bool is_running() const override { return running; }
  virtual void debug_dump(FILE* fp) const override
  {
    fprintf(fp, " %s \r\n", name.c_str());
    size_t n_lcores = n_vcores();
    for (size_t i=0; i<n_lcores; i++) {
      fprintf(fp, "  vlcore[%zd]: plcore%zd \r\n", i, vcore_id_2_lcore_id(i));
    }
  }
  virtual void set_coremask_impl(uint32_t coremask) override
  {
    size_t n_lcores = slankdev::popcnt32(coremask);
    for (size_t i=0; i<n_lcores; i++) {
      size_t rxaid = port_request_rx_access(port_id);
      set_lcore_port_rxaid(i, port_id, rxaid);

      size_t n_port = n_ports();
      for (size_t pid=0; pid<n_port; pid++) {
        size_t txaid = port_request_tx_access(pid);
        set_lcore_port_txaid(i, pid, txaid);
      }
    }
  }
  virtual void undeploy_impl() override { running = false; }
  virtual void deploy_impl(void*) override
  {
    size_t plid = ssn_lcore_id();
    size_t vlid  = get_vlcore_id();
    running = true;
    while (running) {
      rte_mbuf* mbufs[32];
      size_t rxaid = get_lcore_port_rxaid(vlid, port_id);

      if (false) { /* DEBUG CODE */
        size_t next_qid = ssn_ma_port_get_next_rxqid_from_aid(port_id, rxaid);
        printf("rxburst pid=%zd, qid=%zd \n", port_id, next_qid);
      }

      size_t n_recv = rx_burst(port_id, rxaid, mbufs, 32);
      if (n_recv == 0) continue;

      for (size_t i=0; i<n_recv; i++) {
        printf("recv port%zd \n", port_id);

        /* Delay Block begin */
        size_t n=10;
        for (size_t j=0; j<100; j++) n++;

        size_t oport_id = get_oportid_from_iportid(port_id);
        size_t txaid = get_lcore_port_txaid(vlid, oport_id);
        tx_burst(oport_id, txaid, &mbufs[i], 1);
      }
    }
  }
};
class vnf_test : public vnf {
 public:
  vnf_test(size_t n_ports) : vnf(n_ports)
  {
    vnf_impl* vnf_impl0 = new vnf_impl_port(0, "vnf_impl_port0", ports);
    vnf_impl* vnf_impl1 = new vnf_impl_port(1, "vnf_impl_port1", ports);
    vnf_impl* vnf_impl2 = new vnf_impl_port(2, "vnf_impl_port2", ports);
    vnf_impl* vnf_impl3 = new vnf_impl_port(3, "vnf_impl_port3", ports);
    this->add_impl(vnf_impl0);
    this->add_impl(vnf_impl1);
    this->add_impl(vnf_impl2);
    this->add_impl(vnf_impl3);
  }
};

/*---------------------------------------------------------------------------*/

int main(int argc, char** argv)
{
  constexpr size_t n_ports_wanted = 4;
  constexpr size_t n_rxq = 8;
  constexpr size_t n_txq = 8;

  ssn_init(argc, argv);
  size_t n_ports = ssn_dev_count();
  if (n_ports != n_ports_wanted) {
    std::string err = slankdev::format("n_ports is not %zd (current %zd)",
        n_ports_wanted, ssn_dev_count());
    throw slankdev::exception(err.c_str());
  }

  ssn_vnf_port* port[4];
  port[0] = new ssn_vnf_port(0, n_rxq, n_txq); // dpdk0
  port[1] = new ssn_vnf_port(1, n_rxq, n_txq); // dpdk1
  port[2] = new ssn_vnf_port(2, n_rxq, n_txq); // dpdk2
  port[3] = new ssn_vnf_port(3, n_rxq, n_txq); // dpdk3
  printf("\n");
  port[0]->debug_dump(stdout); printf("\n");
  port[1]->debug_dump(stdout); printf("\n");

  /*--------deploy-field-begin----------------------------------------------*/

  vnf_test v0(4);
  v0.attach_port(0, port[0]);
  v0.attach_port(1, port[1]);
  v0.attach_port(2, port[2]);
  v0.attach_port(3, port[3]);

  /* configuration 2 */
  v0.set_coremask(0, 0x02); /* 0b00000010:0x02 */
  v0.set_coremask(1, 0x04); /* 0b00000100:0x04 */
  v0.set_coremask(2, 0x08); /* 0b00001000:0x08 */
  v0.set_coremask(3, 0x10); /* 0b00010000:0x0f */
  v0.config_port_acc();
  v0.deploy();
  v0.debug_dump(stdout);
  getchar();
  v0.undeploy();

#if 0
  /* Reset */
  port[0]->reset_acc(); port[1]->reset_acc(); printf("\n\n");

  /* configuration 3 */
  v0.set_coremask(0, 0x06); /* 0b00000110:0x06 */
  v0.set_coremask(1, 0x08); /* 0b00001000:0x08 */
  v0.config_port_acc();
  v0.deploy();
  v0.debug_dump(stdout);
  getchar();
  v0.undeploy();

  /* Reset */
  port[0]->reset_acc(); port[1]->reset_acc(); printf("\n\n");

  /* configuration 4 */
  v0.set_coremask(0, 0x06); /* 0b00000110:0x06 */
  v0.set_coremask(1, 0x18); /* 0b00011000:0x18 */
  v0.config_port_acc();
  v0.deploy();
  v0.debug_dump(stdout);
  getchar();
  v0.undeploy();
#endif

  /*--------deploy-field-end------------------------------------------------*/

  delete port[0];
  delete port[1];
  delete port[2];
  delete port[3];
  ssn_fin();
}


