
/*
 * MIT License
 *
 * Copyright (c) 2018 Susanow
 * Copyright (c) 2018 Hiroki SHIROKURA
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
#include <ssn_common.h>
#include <ssn_log.h>
#include <dpdk/dpdk.h>
#include <ssn_vnf.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * Support Only 5-tuple ACL
 * - ip protocol
 * - ip src address
 * - ip dst address
 * - l4 src port
 * - l4 dst port
 */
class acl_5tuple {
  struct rte_acl_ctx* acx;
 public:
  acl_5tuple();
  virtual ~acl_5tuple();
  void add_rule(
      uint32_t userdata, uint32_t category_mask,
      uint32_t priority, uint8_t proto,
      uint32_t src_addr, uint8_t src_addr_pref,
      uint32_t dst_addr, uint8_t dst_addr_pref,
      uint16_t src_port_min, uint16_t src_port_max,
      uint16_t dst_port_min, uint16_t dst_port_max);
  void build();
  bool packet_filter_pass(rte_mbuf* mbuf);
  size_t packet_filter_bulk(rte_mbuf** mbufs, size_t n_mbufs,
      rte_mbuf** pass_mbufs, rte_mbuf** eject_mbufs);
};


class ssn_vnf_acl_block : public ssn_vnf_block {
  acl_5tuple& acl;
  size_t get_oportid_from_iportid(size_t in_port_id) { return in_port_id^1; }
  bool running = false;
 public:
  ssn_vnf_acl_block(
      slankdev::fixed_size_vector<ssn_vnf_port*>& ports,
      const char* n, acl_5tuple& acl_)
    : ssn_vnf_block(ports, n), acl(acl_) {}
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
}; /* class ssn_vnf_acl_block */


class ssn_vnf_acl : public ssn_vnf {
  acl_5tuple acl;
 public:
  ssn_vnf_acl(const char* name) : ssn_vnf(2, name)
  {
    acl.add_rule(1, 1, 1, 0x01,
        0x00000000,  0, 0x00000000, 0,
        0x0000, 0xffff, 0x0000, 0xffff);
    acl.add_rule(2, 1, 2, 0x11,
        0xc0a80000, 16, 0xc0a80300, 24,
        0xeeee, 0xeeee, 0x0000, 0xffff);
    acl.build();

    std::string bname = name;
    bname += "block0";
    ssn_vnf_block* block =
      new ssn_vnf_acl_block(ports, bname.c_str(), acl);
    blocks.push_back(block);
  }
  ~ssn_vnf_acl()
  {
    auto* p = blocks.at(blocks.size()-1);
    delete p;
    blocks.pop_back();
  }
}; /* class ssn_vnf_acl */


inline ssn_vnf*
ssn_vnfalloc_acl(const char* instance_name)
{ return new ssn_vnf_acl(instance_name); }


inline void ssn_vnf_acl_block::deploy_impl(void*)
{
  size_t lcore_id = dpdk::lcore_id();
  size_t vcore_id  = get_vlcore_id();

  running = true;
  const size_t n_port = this->n_ports();
  while (running) {
    for (size_t pid=0; pid<n_port; pid++) {
      rte_mbuf* mbufs[32];
      size_t rxaid = get_lcore_port_rxaid(vcore_id, pid);
      size_t txaid = get_lcore_port_txaid(vcore_id, pid^1);

      size_t n_recv = rx_burst(pid, rxaid, mbufs, 32);
      if (n_recv == 0) continue;

      for (size_t i=0; i<n_recv; i++) {
        /*
         * Packet Filter
         */
        static uint32_t cnt = 0;
        bool ret = acl.packet_filter_pass(mbufs[i]);
          printf("0x%04x --> %s\n", cnt++, ret?"pass":"drop");
        if (!ret) {
          dpdk::hexdump_mbuf(stdout, mbufs[i]);
          rte_pktmbuf_free(mbufs[i]);
          continue;
        }

        size_t n_send = tx_burst(pid^1, txaid, &mbufs[i], 1);
        if (n_send < n_recv) {
          dpdk::rte_pktmbuf_free_bulk(&mbufs[n_send], n_recv-n_send);
        }
      }

    } /* for (size_t pid=0; pid<n_port; pid++) */
  } /* while (running) */
}


