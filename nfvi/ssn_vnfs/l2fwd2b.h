
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


class ssn_vnf_l2fwd2b_block_port : public ssn_vnf_block {
  size_t get_oportid_from_iportid(size_t in_port_id) { return in_port_id^1; }
 public:
  bool running = false;
  const size_t port_id;

  ssn_vnf_l2fwd2b_block_port(size_t poll_pid,
      slankdev::fixed_size_vector<ssn_vnf_port*>& ports, const char* n)
    : ssn_vnf_block(ports, n)
    , port_id(poll_pid) {}

  virtual bool is_running() const override { return running; }
  virtual void undeploy_impl() override { running = false; }
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
  virtual void deploy_impl(void*) override;
  virtual void debug_dump(FILE* fp) const override;
}; /* class ssn_vnf_l2fwd2b_block_port */

class ssn_vnf_l2fwd2b : public ssn_vnf {
 public:
  ssn_vnf_l2fwd2b(const char* n) : ssn_vnf(2, n)
  {
    std::string name0 = slankdev::format("%sblockport0", n);
    std::string name1 = slankdev::format("%sblockport1", n);
    ssn_vnf_block* vnf_block0 = new ssn_vnf_l2fwd2b_block_port(0, ports, name0.c_str());
    ssn_vnf_block* vnf_block1 = new ssn_vnf_l2fwd2b_block_port(1, ports, name1.c_str());
    this->blocks.push_back(vnf_block0);
    this->blocks.push_back(vnf_block1);
  }
}; /* class ssn_vnf_l2fwd2b */

inline ssn_vnf*
ssn_vnfalloc_l2fwd2b(const char* instance_name)
{ return new ssn_vnf_l2fwd2b(instance_name); }


