
/*
 * MIT License
 *
 * Copyright (c) 2017 Susanow Project
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

#include <ssn_vnf.h>
#include <ssn_port.h>
#include <ssn_port_stat.h>
#include <ssn_ma_port.h>
#include <ssn_log.h>
#include <ssn_cpu.h>
#include <ssn_common.h>
#include <ssn_thread.h>

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <vector>
#include <assert.h>
#include <slankdev/string.h>
#include <slankdev/exception.h>


ssn_vnf_port::ssn_vnf_port(size_t a_port_id, size_t a_n_rxq, size_t a_n_txq) :
  port_id(a_port_id), n_rxq(a_n_rxq), n_txq(a_n_txq), n_rxacc(0), n_txacc(0)
{
  ssn_ma_port_configure_hw(port_id, n_rxq, n_txq);
  ssn_ma_port_dev_up(port_id);
  ssn_ma_port_promisc_on(port_id);
}

void ssn_vnf_port::configure(size_t n_rxacc_, size_t n_txacc_)
{
  n_rxacc = n_rxacc_;
  n_txacc = n_txacc_;
  ssn_ma_port_configure_acc(port_id, n_rxacc, n_txacc);
}

size_t ssn_vnf_port::tx_burst(size_t aid, rte_mbuf** mbufs, size_t n_mbufs)
{ return ssn_ma_port_tx_burst(port_id, aid, mbufs, n_mbufs); }

size_t ssn_vnf_port::rx_burst(size_t aid, rte_mbuf** mbufs, size_t n_mbufs)
{ return ssn_ma_port_rx_burst(port_id, aid, mbufs, n_mbufs); }

void ssn_vnf_port::debug_dump(FILE* fp) const
{
  fprintf(fp, " port_id: %zd\r\n", port_id);
  fprintf(fp, " n_rxq  : %zd\r\n", n_rxq  );
  fprintf(fp, " n_txq  : %zd\r\n", n_txq  );
  fprintf(fp, " n_rxacc: %zd\r\n", n_rxacc);
  fprintf(fp, " n_txacc: %zd\r\n", n_txacc);
}


void _vnf_thread_spawner(void* vnf_instance_)
{
  ssn_vnf* vnf = reinterpret_cast<ssn_vnf*>(vnf_instance_);
  vnf->deploy_imple(nullptr);
}

size_t ssn_vnf::tx_burst(size_t pid, size_t aid, rte_mbuf** mbufs, size_t n_mbufs)
{ return ports[pid]->tx_burst(aid, mbufs, n_mbufs); }

size_t ssn_vnf::rx_burst(size_t pid, size_t aid, rte_mbuf** mbufs, size_t n_mbufs)
{ return ports[pid]->rx_burst(aid, mbufs, n_mbufs); }

ssn_vnf::ssn_vnf(size_t n_ports_a) : ports(n_ports_a) {}

ssn_vnf::~ssn_vnf() {}

size_t ssn_vnf::n_ports() const { return ports.size(); }

void ssn_vnf::attach_port(size_t pid, ssn_vnf_port* p) { ports.at(pid) = p; }

void ssn_vnf::deploy(uint32_t lcore_mask)
{
  this->coremask = lcore_mask;

  lcores.clear();
  for (size_t i=0; i<32; i++) {
    if ((lcore_mask & (0x1<<i)) != 0)
      lcores.push_back(i);
  }
  tids.resize(lcores.size());

  for (size_t i=0; i<ports.size(); i++) {
    ports[i]->configure(lcores.size(), lcores.size());
  }

  for (size_t i=0; i<lcores.size(); i++) {
    tids[i] = ssn_thread_launch(_vnf_thread_spawner, this, lcores[i]);
  }
}

void ssn_vnf::undeploy()
{
  assert(tids.size() == lcores.size());

  undeploy_imple();
  for (size_t i=0; i<lcores.size(); i++) {
    ssn_thread_join(tids[i]);
  }
  printf("undeploy succes\n");
}

size_t ssn_vnf::get_aid_from_lcoreid(size_t lcore_id) const
{
  size_t n_lcores = lcores.size();
  for (size_t i=0; i<n_lcores; i++) {
    if (lcores[i] == lcore_id) return i;
  }
  throw slankdev::exception(
      "vnf::get_aid_from_lcoreid: not found aid (lcoreid is invalid?)");
}

size_t ssn_vnf::get_aid() const
{
  size_t lcore_id = ssn_lcore_id();
  return get_aid_from_lcoreid(lcore_id);
}

void ssn_vnf::debug_dump(FILE* fp) const
{
  fprintf(fp, " state   : %s \r\n", this->is_running()?"running":"stop");
  fprintf(fp, " coremask: ");
  if (is_running()) {
    fprintf(fp, "0x%04x ", coremask);
    fprintf(fp, "%s \r\n", slankdev::ntob<8>(coremask).c_str());
    auto n = tids.size();
    for (size_t i=0; i<n; i++) {
      fprintf(fp, "    tid[%zd] 0x%08x \r\n", i, tids[i]);
    }
  } else {
    fprintf(fp, "-\r\n");
  }
  fprintf(fp, " n_ports : %zd \r\n", this->n_ports());
}


