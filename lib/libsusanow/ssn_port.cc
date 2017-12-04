
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

#include <stdio.h>
#include <ssn_log.h>
#include <ssn_port.h>

#include <dpdk/hdr.h>
#include <dpdk/wrap.h>
#include <dpdk/struct.h>

#include <slankdev/exception.h>
#include <slankdev/string.h>

#if 0
rte_mempool* mp[RTE_MAX_ETHPORTS];
#endif

size_t ssn_dev_count()
{
  return rte_eth_dev_count();
}

void ssn_port_link_up(size_t p)
{
  int ret = rte_eth_dev_set_link_up(p);
  if (ret < 0) {
    ssn_log(SSN_LOG_CRIT, "link up miss\n");
    throw slankdev::exception("ssn_port_link_up");
  }
  ssn_log(SSN_LOG_INFO, "port%zd link up\n", p);
}

void ssn_port_link_down(size_t p)
{
  ssn_log(SSN_LOG_INFO, "port%zd link down\n", p);
  rte_eth_dev_set_link_down(p);
}

void ssn_port_promisc_on(size_t pid)
{
  rte_eth_promiscuous_enable(pid);
}

void ssn_port_promisc_off(size_t pid)
{
  rte_eth_promiscuous_disable(pid);
}

void ssn_port_dev_up(size_t pid)
{
  int ret = rte_eth_dev_start(pid);
  if (ret < 0) {
    throw slankdev::exception("eth dev start");
  }
  ssn_log(SSN_LOG_INFO, "port%zd dev up\n", pid);
}

void ssn_port_dev_down(size_t pid)
{
  rte_eth_dev_stop(pid);
  ssn_log(SSN_LOG_INFO, "port%zd dev down\n", pid);
}

void ssn_port_conf::debug_dump(FILE* fp) const
{
  using namespace slankdev;
  using namespace dpdk;

  fprintf(fp, "nb_rxq: %zd \r\n", nb_rxq);
  fprintf(fp, "nb_txq: %zd \r\n", nb_txq);
  fprintf(fp, "nb_rxd: %zd \r\n", nb_rxd);
  fprintf(fp, "nb_txd: %zd \r\n", nb_txd);

  fprintf(fp, "raw {\r\n");
  fprintf(fp, "  link_speeds : %u(%s) \r\n", raw.link_speeds, linkspeed2str(raw.link_speeds));
  fprintf(fp, "  rxmode {\r\n");

  fprintf(fp, "    mq_mode        : %s\n", rte_eth_rx_mq_mode2str(raw.rxmode.mq_mode));
  fprintf(fp, "    max_rx_pkt_len : %u\n", raw.rxmode.max_rx_pkt_len);
  fprintf(fp, "    split_hdr_size : %u\n", raw.rxmode.split_hdr_size);
  fprintf(fp, "    header_split   : %s\n", (raw.rxmode.header_split  ==1)?"Yes":"No");
  fprintf(fp, "    hw_ip_checksum : %s\n", (raw.rxmode.hw_ip_checksum==1)?"Yes":"No");
  fprintf(fp, "    hw_vlan_filter : %s\n", (raw.rxmode.hw_vlan_filter==1)?"Yes":"No");
  fprintf(fp, "    hw_vlan_strip  : %s\n", (raw.rxmode.hw_vlan_strip ==1)?"Yes":"No");
  fprintf(fp, "    hw_vlan_extend : %s\n", (raw.rxmode.hw_vlan_extend==1)?"Yes":"No");
  fprintf(fp, "    jumbo_frame    : %s\n", (raw.rxmode.jumbo_frame   ==1)?"Yes":"No");
  fprintf(fp, "    hw_strip_crc   : %s\n", (raw.rxmode.hw_strip_crc  ==1)?"Yes":"No");
  fprintf(fp, "    enable_scatter : %s\n", (raw.rxmode.enable_scatter==1)?"Yes":"No");
  fprintf(fp, "    enable_lro     : %s\n", (raw.rxmode.enable_lro    ==1)?"Yes":"No");
  fprintf(fp, "  }\r\n");

  fprintf(fp, "  txmode \r\n");
  fprintf(fp, "  rx_adv_conf {\r\n");
  fprintf(fp, "    rss_conf {       \r\n");
  fprintf(fp, "      rss_key    : %p \r\n", raw.rx_adv_conf.rss_conf.rss_key);
  fprintf(fp, "      rss_key_len: %u \r\n", raw.rx_adv_conf.rss_conf.rss_key_len);
  fprintf(fp, "      rss_hf     : 0x%016lx\r\n", raw.rx_adv_conf.rss_conf.rss_hf);
  fprintf(fp, "       \r\n");
  fprintf(fp, "    }                \r\n");
  fprintf(fp, "    vmdq_dcb_conf    \r\n");
  fprintf(fp, "    dcb_rx_conf      \r\n");
  fprintf(fp, "    vmdq_rx_conf     \r\n");
  fprintf(fp, "  }\r\n");
  fprintf(fp, "  tx_adv_conf {\r\n");
  fprintf(fp, "    vmdq_dcb_tx_conf \r\n");
  fprintf(fp, "    dcb_tx_conf      \r\n");
  fprintf(fp, "    vmdq_tx_conf     \r\n");
  fprintf(fp, "  }\r\n");
  fprintf(fp, "  dcb_capability_en %u \r\n", raw.dcb_capability_en  );
  fprintf(fp, "  fdir_conf \r\n");
  fprintf(fp, "  intr_conf \r\n");
  fprintf(fp, "}\r\n");
}

ssn_port_conf::ssn_port_conf()
  : nb_rxq(1), nb_txq(1), nb_rxd(128), nb_txd(512)
{
  dpdk::init_portconf(&raw);
}

void ssn_port_configure(size_t pid, ssn_port_conf* conf, struct rte_mempool* mp)
{
  int ret;
  ssn_log(SSN_LOG_INFO,
      "port%zd configure nb_rxqs=%zd, nb_txqs=%zd, nb_rxd=%zd, nb_txd=%zd\n",
      pid, conf->nb_rxq, conf->nb_txq, conf->nb_rxd, conf->nb_txd);

  ret = rte_eth_dev_configure(pid, conf->nb_rxq, conf->nb_txq, &conf->raw);
  if (ret < 0) {
    throw slankdev::exception("dev configure");
  }
  for (size_t q=0; q<conf->nb_rxq; q++) {
    ret = rte_eth_rx_queue_setup(pid, q, conf->nb_rxd, rte_eth_dev_socket_id(pid), nullptr, mp);
    if (ret < 0) {
      std::string errstr = slankdev::format(
          "rte_eth_rx_queue_setup(%zd,%zd,%zd,%zd,%p,%p)",
          pid, q, conf->nb_rxd, rte_eth_dev_socket_id(pid), nullptr, mp);
      throw slankdev::exception(errstr.c_str());
    }
  }
  for (size_t q=0; q<conf->nb_txq; q++) {
    ret = rte_eth_tx_queue_setup(pid, q, conf->nb_txd, rte_eth_dev_socket_id(pid), nullptr);
    if (ret < 0) {
      throw slankdev::exception("dev txq setup");
    }
  }
}

void ssn_port_init()
{
#if 0
  size_t nb_ports = rte_eth_dev_count();
  for (size_t i=0; i<nb_ports; i++) {
    std::string name = slankdev::format("RXMP%zd", i);
    // mp[i] = dpdk::mp_alloc(name.c_str());
  }
#endif
}

void ssn_port_fin()
{
  size_t nb_ports = rte_eth_dev_count();
  for (size_t i=0; i<nb_ports; i++) {
#if 0
    rte_mempool_free(mp[i]);
#endif
    ssn_port_link_down(i);
    ssn_port_dev_down(i);
  }
}

void ssn_mbuf_free_bulk(rte_mbuf** m_list, size_t npkts)
{
  while (npkts--)
    rte_pktmbuf_free(*m_list++);
}

size_t ssn_port_rx_burst(size_t pid, size_t qid, rte_mbuf** mbufs, size_t nb_mbufs)
{
  size_t nb_recv = rte_eth_rx_burst(pid, qid, mbufs, nb_mbufs);
  return nb_recv;
}

size_t ssn_port_tx_burst(size_t pid, size_t qid, rte_mbuf** mbufs, size_t nb_mbufs)
{
  size_t nb_send = rte_eth_tx_burst(pid, qid, mbufs, nb_mbufs);
  return nb_send;
}

