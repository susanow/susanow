


/*-
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
/**
 * @file ssnlib_port_impl.cc
 * @author slankdev
 */


#include <ssnlib_port_impl.h>

#include <rte_ring.h>
#include <rte_config.h>
#include <rte_version.h>
#include <rte_eal.h>
#include <rte_ether.h>
#include <rte_cycles.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>
#include <rte_hexdump.h>
#include <rte_ip.h>
#include <rte_ip_frag.h>

#include <slankdev/system.h>



port_conf::port_conf(size_t i) : id(i)
{
  memset(&raw, 0x00, sizeof(raw));
  raw.rxmode.max_rx_pkt_len = ETHER_MAX_LEN;
}


void port_stats::reset()
{
  rte_eth_stats_reset(id);
  rte_eth_stats_get(id, &init);
  cure = init;
  cure_prev = init;
  last_update = slankdev::rdtsc();
}
void port_stats::update()
{
  rte_eth_stats_get(id, &cure);
  uint64_t now = slankdev::rdtsc();
  double diff_time = double(now-last_update)/rte_get_timer_hz();
  rx_pps = (cure.ipackets - cure_prev.ipackets)/diff_time;
  tx_pps = (cure.opackets - cure_prev.opackets)/diff_time;
  rx_bps = (((cure.ibytes - cure_prev.ibytes) + IFO * rx_pps) << 3)/diff_time;
  tx_bps = (((cure.obytes - cure_prev.obytes) + IFO * tx_pps) << 3)/diff_time;
  cure_prev = cure;
  last_update = now;
}



std::string Ether_addr::toString() const
{
  char buf[32];
  snprintf(buf, sizeof(buf),
      "%02" PRIx8 ":%02" PRIx8 ":%02" PRIx8
      ":%02" PRIx8 ":%02" PRIx8 ":%02" PRIx8,
      addr_bytes[0], addr_bytes[1],
      addr_bytes[2], addr_bytes[3],
      addr_bytes[4], addr_bytes[5]);
  return buf;
}



void Ether_addr::set(::ether_addr* addr)
{
  int ret = rte_eth_dev_default_mac_addr_set(id, addr);
  if (ret < 0) {
    if (ret == -ENOTSUP) {
      throw slankdev::exception(
          "rte_eth_dev_default_mac_addr_set: hardware doesn't suppoer");
    } else if (ret == -ENODEV) {
      throw slankdev::exception(
          "rte_eth_dev_default_mac_addr_set: port invalid");
    } else if (ret == -EINVAL) {
      throw slankdev::exception(
          "rte_eth_dev_default_mac_addr_set: MAC address is invalid");
    } else {
      throw slankdev::exception(
          "rte_eth_dev_default_mac_addr_set: unknown error");
    }
  }
  update();
}
void Ether_addr::add(::ether_addr* addr)
{
  int ret = rte_eth_dev_mac_addr_add(id, addr, 0);
  if (ret < 0) {
    if (ret == -ENOTSUP) {
      throw slankdev::exception(
          "rte_eth_dev_mac_addr_add: hardware doesn't support this feature.");
    } else if (ret == -ENODEV) {
      throw slankdev::exception(
          "rte_eth_dev_mac_addr_add: port is invalid.");
    } else if (ret == -ENOSPC) {
      throw slankdev::exception(
          "rte_eth_dev_mac_addr_add: no more MAC addresses can be added.");
    } else if (ret == -EINVAL) {
      throw slankdev::exception(
          "rte_eth_dev_mac_addr_add: MAC address is invalid.");
    } else {
      throw slankdev::exception("rte_eth_dev_mac_addr_add: unknown");
    }
  }
  update();
}
void Ether_addr::del(::ether_addr* addr)
{
  int ret = rte_eth_dev_mac_addr_remove(id, addr);
  if (ret < 0) {
    if (ret == -ENOTSUP) {
      throw slankdev::exception(
          "rte_eth_dev_mac_addr_remove: hardware doesn't support.");
    } else if (ret == -ENODEV) {
      throw slankdev::exception(
          "rte_eth_dev_mac_addr_remove: if port invalid.");
    } else if (ret == -EADDRINUSE) {
      std::string errstr = "rte_eth_dev_mac_addr_remove: ";
      errstr += "attempting to remove the default MAC address";
      throw slankdev::exception(errstr.c_str());
    }
  }
  update();
}
