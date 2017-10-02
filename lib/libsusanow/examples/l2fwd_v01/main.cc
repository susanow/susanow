
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
#include <slankdev/string.h>
#include <slankdev/exception.h>

#include <ssn_port.h>
#include <ssn_common.h>
#include <ssn_log.h>
#include <ssn_vnf_v01.h>
#include <dpdk/dpdk.h>


size_t get_oportid_from_iportid(size_t in_port_id) { return in_port_id^1; }

class vnf_block : public ssn_vnf_block {
  bool running = false;
 public:
  vnf_block(fixed_size_vector<ssn_vnf_port*>& ports) : ssn_vnf_block(ports) {}
  virtual bool is_running() const override { return running; }
  virtual void undeploy_impl() override { running = false; }
  virtual void debug_dump(FILE* fp) const override { throw slankdev::exception("NOTIMPL"); }
  virtual void set_coremask_impl(uint32_t coremask) override
  {
    size_t n_vcores = slankdev::popcnt32(coremask);
    for (size_t i=0; i<n_vcores; i++) {
      size_t n_port = n_ports();
      for (size_t pid=0; pid<n_port; pid++) {
        size_t rxaid = port_request_rx_access(pid);
        set_lcore_port_rxaid(i, pid, rxaid);
        size_t txaid = port_request_tx_access(pid);
        set_lcore_port_txaid(i, pid, txaid);
      }
    }
  }
  virtual void deploy_impl(void*) override
  {
    size_t lcore_id = ssn_lcore_id();
    size_t vcore_id  = get_vlcore_id();

    running = true;
    while (running) {
      size_t n_port = this->n_ports();
      for (size_t pid=0; pid<n_port; pid++) {
        rte_mbuf* mbufs[32];
        size_t rxaid = get_lcore_port_rxaid(vcore_id, pid);
        size_t txaid = get_lcore_port_txaid(vcore_id, pid^1);

        size_t n_recv = rx_burst(pid, rxaid, mbufs, 32);
        if (n_recv == 0) continue;

        for (size_t i=0; i<n_recv; i++) {

          /* Delay Block begin */
          size_t n=10;
          for (size_t j=0; j<100; j++) n++;

          tx_burst(pid^1, txaid, &mbufs[i], 1);
        }
      } //for
    } /* while (running) */
  }
};

class vnf : public ssn_vnf {
 public:
  vnf() : ssn_vnf(2)
  {
    ssn_vnf_block* block = new vnf_block(ports);
    this->add_block(block);
  }
}; /* class vnf */

int main(int argc, char** argv)
{
  ssn_init(argc, argv);
  size_t n_ports = ssn_dev_count();
  if (n_ports != 2) {
    std::string err = slankdev::format("n_ports is not 2 (current %zd)",
        ssn_dev_count());
    throw slankdev::exception(err.c_str());
  }

  ssn_vnf_port* port0 = new ssn_vnf_port(0, 4, 4); // dpdk0
  ssn_vnf_port* port1 = new ssn_vnf_port(1, 4, 4); // dpdk1
  printf("\n");
  port0->debug_dump(stdout); printf("\n");
  port1->debug_dump(stdout); printf("\n");

  vnf v;
  v.attach_port(0, port0);
  v.attach_port(1, port1);

  //-------------------------------------------------------

  port0->reset_acc();
  port1->reset_acc();
  v.set_coremask(0, 0x02); /* 0b00000010:0x02 */
  v.config_port_acc();
  v.deploy();
  getchar();
  v.undeploy();

  //-------------------------------------------------------

  port0->reset_acc();
  port1->reset_acc();
  v.set_coremask(0, 0x06); /* 0b00000110:0x06 */
  v.config_port_acc();
  v.deploy();
  getchar();
  v.undeploy();

  //-------------------------------------------------------

  port0->reset_acc();
  port1->reset_acc();
  v.set_coremask(0, 0x1e); /* 0b00011110:0x1e */
  v.config_port_acc();
  v.deploy();
  getchar();
  v.undeploy();

fin:
  delete port0;
  delete port1;
  ssn_fin();
}


