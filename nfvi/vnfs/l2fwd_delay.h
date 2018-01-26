
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
#include <ssn_port.h>
#include <ssn_common.h>
#include <ssn_log.h>
#include <dpdk/dpdk.h>
#include <ssn_vnf.h>


class ssn_vnf_l2fwd_delay_block : public ssn_vnf_block {
  size_t get_oportid_from_iportid(size_t in_port_id) { return in_port_id^1; }
  bool running = false;
 public:
  ssn_vnf_l2fwd_delay_block(
      slankdev::fixed_size_vector<ssn_vnf_port*>& ports, const char* n)
    : ssn_vnf_block(ports, n)
  { printf("create l2fwd_delay n_delay=%zd \n", n_delay); }
  virtual bool is_running() const override { return running; }
  virtual void undeploy_impl() override { running = false; }
  virtual void debug_dump(FILE* fp) const override { fprintf(fp, "non\r\n"); }
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
  virtual void deploy_impl(void*) override;
  static size_t n_delay;
};

class ssn_vnf_l2fwd_delay : public ssn_vnf {
 public:

  ssn_vnf_l2fwd_delay(const char* name) : ssn_vnf(2, name)
  {
    std::string bname = name;
    bname += "block0";
    ssn_vnf_block* block = new ssn_vnf_l2fwd_delay_block(ports, bname.c_str());
    blocks.push_back(block);
  }
  ~ssn_vnf_l2fwd_delay()
  {
    auto* p = blocks.at(blocks.size()-1);
    delete p;
    blocks.pop_back();
  }
}; /* class ssn_vnf_l2fwd */

inline ssn_vnf*
ssn_vnfalloc_l2fwd_delay(const char* instance_name)
{ return new ssn_vnf_l2fwd_delay(instance_name); }


inline void ssn_vnf_l2fwd_delay_block::deploy_impl(void*)
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
        for (size_t j=0; j<ssn_vnf_l2fwd_delay_block::n_delay; j++) n++;

      }
      size_t n_send = tx_burst(pid^1, txaid, mbufs, n_recv);
      if (n_send < n_recv) {
        dpdk::rte_pktmbuf_free_bulk(&mbufs[n_send], n_recv-n_send);
      }

    } /* for */
  } /* while (running) */
}


