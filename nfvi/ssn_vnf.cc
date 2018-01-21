
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

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <slankdev/string.h>
#include <slankdev/exception.h>
#include <slankdev/util.h>
#include <exception>

#include <ssn_port_stat.h>
#include <ssn_ma_port.h>
#include <ssn_ma_ring.h>
#include <ssn_thread.h>
#include <ssn_cpu.h>
#include <ssn_port.h>
#include <ssn_common.h>
#include <ssn_log.h>
#include <dpdk/dpdk.h>
#include <slankdev/vector.h>
#include <ssn_vnf_port.h>
#include <ssn_vnf_vcore.h>
#include <ssn_vnf_block.h>
#include <ssn_vnf.h>


size_t ssn_vnf::n_ports() const { return ports.size(); }
size_t ssn_vnf::n_blocks() const { return blocks.size(); }
const ssn_vnf_block* ssn_vnf::get_block(size_t bid) const { return blocks.at(bid); }
const ssn_vnf_port* ssn_vnf::get_port(size_t pid) const { return ports.at(pid); }


void ssn_vnf::configre_acc()
{
  size_t n_ports = ports.size();
  for (size_t i=0; i<n_ports; i++) {
    ports.at(i)->config_acc();
  }
}

bool ssn_vnf::deployable() const
{
  /*
   * Condition
   *  - port is attached
   *  - all coremask is set
   */

  /*
   * check ports
   */
  size_t n_port = ports.size();
  for (size_t i=0; i<n_port; i++) {
    if (ports[i] == nullptr) return false;
  }

  /*
   * check coremask
   */
  size_t n_block = n_blocks();
  for (size_t i=0; i<n_block; i++) {
    if (blocks[i]->get_coremask() == 0) return false;
  }

  return true;
}

int ssn_vnf::reset()
{
  /*
   * Reset coremask
   */
  const size_t n_ele = blocks.size();
  for (size_t i=0; i<n_ele; i++) {
    blocks.at(i)->reset();
    set_coremask(i, 0x00);
  }

  /*
   * Reset all port accessor
   */
  const size_t n_port = ports.size();
  for (size_t i=0; i<n_port; i++) {
    if (!ports.at(i)) return -1;
  }
  for (size_t i=0; i<n_port; i++) {
    ports.at(i)->reset_acc();
  }
  return 0;
}

ssn_vnf::ssn_vnf(size_t nport, const char* n) : ports(nport), name(n)
{
  const size_t n_ele = ports.size();
  for (size_t i=0; i<n_ele; i++) {
    ports.at(i) = nullptr;
  }
}

uint32_t ssn_vnf::get_coremask(size_t bid) const
{
  return blocks.at(bid)->get_coremask();
}

uint32_t ssn_vnf::get_coremask() const
{
  uint32_t coremask = 0;
  const size_t n_ele = n_blocks();
  for (size_t i=0; i<n_ele; i++) {
    coremask |= blocks.at(i)->get_coremask();
  }
  return coremask;
}

int ssn_vnf::set_coremask(size_t block_id, uint32_t cmask)
{
  const size_t n_port = ports.size();
  for (size_t i=0; i<n_port; i++) {
    if (!ports.at(i)) return -1;
  }
  blocks.at(block_id)->set_coremask(cmask);
  return 0;
}

bool ssn_vnf::is_deletable() const
{
  const size_t n_ele = ports.size();
  for (size_t i=0; i<n_ele; i++) {
    if (ports.at(i) != nullptr) {
      return false;
    }
  }
  return true;
}

void ssn_vnf::debug_dump(FILE* fp) const
{
  using std::string;
  fprintf(fp, "\r\n");
  fprintf(fp, " + name: \"%s\" \r\n", name.c_str());
  fprintf(fp, " + ports\r\n");
  size_t n = ports.size();
  for (size_t i=0; i<n; i++) {
    auto* port = ports.at(i);
    if (port) {
      string name = port->name;
      size_t orx = port->get_outer_rx_perf();
      size_t irx = port->get_inner_rx_perf();
      double r   = port->get_perf_reduction();
      printf("  port[%zd]: name=%s orx=%zd irx=%zd red=%lf\n",
          i, name.c_str(), orx, irx, r);
    } else {
      printf("  port[%zd]: null\n", i);
    }
  }
  fprintf(fp, "\r\n");
}

// TODO need refactoring
int ssn_vnf::deploy()
{
  const size_t n_port = ports.size();
  for (size_t i=0; i<n_port; i++) {
    if (!ports.at(i)) {
      // TODO: return with Error Code
      // printf("port not attached pid=%zd\n", i);
      return -1;
    }
    if (!ports.at(i)->deployable()) return -1;
  }
  configre_acc();
  auto n_impl = blocks.size();
  try {
    for (size_t i=0; i<n_impl; i++) {
      int ret = this->blocks.at(i)->deploy();
      if (ret < 0) {
        // TODO: return with Error Code
        printf("block deploy miss bid=%zd\n", i);
        return -1;
      }
    }
  } catch (std::exception& e) {
    printf("slankdev??\n");
    undeploy();
    return -1;
  }
  return 0;
}

void ssn_vnf::undeploy()
{
  auto n_impl = blocks.size();
  for (size_t i=0; i<n_impl; i++) {
    if (this->blocks.at(i)->is_running()) {
      this->blocks.at(i)->undeploy();
    }
  }
}

void ssn_vnf::update_stats()
{
  auto np = ports.size();
  for (size_t i=0; i<np; i++) {
    this->ports.at(i)->stats_update_per1sec();
  }
}

bool ssn_vnf::is_running() const
{
  auto nb = n_blocks();
  for (size_t i=0; i<nb; i++) {
    if (!blocks.at(i)->is_running()) return false;
  }
  return true;
}


int ssn_vnf::attach_port(size_t pid, ssn_vnf_port* port)
{
  if (ports.at(pid)) {
    /* my-port was already attached */
    return -1;
  }

  if (port->is_attached_vnf()) {
    /* port has attached vnf */
    return -1;
  }

  ports.at(pid) = port;
  port->attach_vnf(this);
  auto n = blocks.size();
  for (size_t i=0; i<n; i++) {
    blocks.at(i)->attach_port(pid, port);
  }
  return 0;
}

int ssn_vnf::dettach_port(size_t pid)
{
  if (!ports.at(pid)) {
    /* unattached port yet */
    return -1;
  }
  ports.at(pid)->dettach_vnf();
  ports.at(pid) = nullptr;
  auto n = blocks.size();
  for (size_t i=0; i<n; i++) {
    blocks.at(i)->dettach_port(pid);
  }
  return 0;
}

double ssn_vnf::get_perf_reduction() const
{
  double pr = 0.0;
  const size_t n_port = ports.size();
  for (size_t i=0; i<n_port;  i++) {
    if (ports.at(i) == nullptr) {
      return 1.0;
    }
    pr += ports.at(i)->get_perf_reduction();
  }
  double ret = pr/n_port;
  if (ret > 1) ret = 1;
  return ret;
}

size_t ssn_vnf::get_rx_rate() const
{
  size_t sum = 0;
  const size_t n_port = ports.size();
  for (size_t i=0; i<n_port; i++) {
    if (!ports.at(i)) return 0;
    sum += ports.at(i)->get_outer_rx_perf();
  }
  return sum;
}
