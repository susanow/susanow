


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
 * @file command/imple.h
 * @author slankdev
 */


#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <ssnlib_sys.h>
#include <ssnlib_sys.h>
#include <ssnlib_thread.h>

#include <slankdev/vty.h>


inline void _port_statistics(slankdev::shell* sh)
{
  System* sys = get_sys(sh);
  // sys->ports[i].stats.


  struct rte_eth_stats st;
  size_t nb_ports = sys->ports.size();
  for (size_t i=0; i<nb_ports; i++) {
    memset(&st, 0, sizeof(st));
    rte_eth_stats_get(i, &st);
    sh->Printf(" Port Statistics pid=%zd\r\n", i);
  }
}


inline void _port_rxmode_show(slankdev::shell* sh)
{
  System* sys = get_sys(sh);
  if (sys->ports.empty()) {
    sh->Printf("Port not found :( \r\n");
    return ;
  }

  const rte_eth_rxmode& rxmode = sys->ports[0].conf.raw.rxmode;
  sh->Printf(" mq_mode        : ");
  switch (rxmode.mq_mode) {
case ETH_MQ_RX_NONE        : sh->Printf("NONE        \r\n"); break;
case ETH_MQ_RX_RSS         : sh->Printf("RSS         \r\n"); break;
case ETH_MQ_RX_DCB         : sh->Printf("DCB         \r\n"); break;
case ETH_MQ_RX_DCB_RSS     : sh->Printf("DCB_RSS     \r\n"); break;
case ETH_MQ_RX_VMDQ_ONLY   : sh->Printf("VMDQ_ONLY   \r\n"); break;
case ETH_MQ_RX_VMDQ_RSS    : sh->Printf("VMDQ_RSS    \r\n"); break;
case ETH_MQ_RX_VMDQ_DCB    : sh->Printf("VMDQ_DCB    \r\n"); break;
case ETH_MQ_RX_VMDQ_DCB_RSS: sh->Printf("VMDQ_DCB_RSS\r\n"); break;
default: assert(false);
  }
sh->Printf(" max_rx_pkt_len : %u\r\n", rxmode.max_rx_pkt_len);
sh->Printf(" split_hdr_size : %u\r\n", rxmode.split_hdr_size);
sh->Printf(" header_split   : %s\r\n", rxmode.header_split  ?"True":"False");
sh->Printf(" hw_ip_checksum : %s\r\n", rxmode.hw_ip_checksum?"True":"False");
sh->Printf(" hw_vlan_filter : %s\r\n", rxmode.hw_vlan_filter?"True":"False");
sh->Printf(" hw_vlan_strip  : %s\r\n", rxmode.hw_vlan_strip ?"True":"False");
sh->Printf(" hw_vlan_extend : %s\r\n", rxmode.hw_vlan_extend?"True":"False");
sh->Printf(" jumbo_frame    : %s\r\n", rxmode.jumbo_frame   ?"True":"False");
sh->Printf(" hw_strip_crc   : %s\r\n", rxmode.hw_strip_crc  ?"True":"False");
sh->Printf(" enable_scatter : %s\r\n", rxmode.enable_scatter?"True":"False");
sh->Printf(" enable_lro     : %s\r\n", rxmode.enable_lro    ?"True":"False");
}


inline void _port_rss(slankdev::shell* sh)
{
  System* sys = get_sys(sh);
  if (sys->ports.size() < 1) {
    sh->Printf("Port not found :( \r\n");
    return ;
  }

  Port& port = sys->ports[0];
  struct rte_eth_rss_conf& conf = port.conf.raw.rx_adv_conf.rss_conf;
  sh->Printf(" key      : %p \r\n", conf.rss_key);
  if (conf.rss_key) {
    for (size_t j=0; j<5; j++) {
      sh->Printf("   ");
      for (size_t i=0; i<8; i++) {
        printf("%02x ", conf.rss_key[8*j + i]);
      }
      sh->Printf("\r\n");
    }
  }
  sh->Printf(" key len  : %u \r\n", conf.rss_key_len);
  sh->Printf(" hash func: ");
  uint64_t hf = conf.rss_hf;
  if (hf & ETH_RSS_IPV4              ) sh->Printf(",IPV4"              );
  if (hf & ETH_RSS_FRAG_IPV4         ) sh->Printf(",FRAG_IPV4"         );
  if (hf & ETH_RSS_NONFRAG_IPV4_TCP  ) sh->Printf(",NONFRAG_IPV4_TCP"  );
  if (hf & ETH_RSS_NONFRAG_IPV4_UDP  ) sh->Printf(",NONFRAG_IPV4_UDP"  );
  if (hf & ETH_RSS_NONFRAG_IPV4_SCTP ) sh->Printf(",NONFRAG_IPV4_SCTP" );
  if (hf & ETH_RSS_NONFRAG_IPV4_OTHER) sh->Printf(",NONFRAG_IPV4_OTHER");
  if (hf & ETH_RSS_IPV6              ) sh->Printf(",IPV6"              );
  if (hf & ETH_RSS_FRAG_IPV6         ) sh->Printf(",FRAG_IPV6"         );
  if (hf & ETH_RSS_NONFRAG_IPV6_TCP  ) sh->Printf(",NONFRAG_IPV6_TCP"  );
  if (hf & ETH_RSS_NONFRAG_IPV6_UDP  ) sh->Printf(",NONFRAG_IPV6_UDP"  );
  if (hf & ETH_RSS_NONFRAG_IPV6_SCTP ) sh->Printf(",NONFRAG_IPV6_SCTP" );
  if (hf & ETH_RSS_NONFRAG_IPV6_OTHER) sh->Printf(",NONFRAG_IPV6_OTHER");
  if (hf & ETH_RSS_L2_PAYLOAD        ) sh->Printf(",L2_PAYLOAD"        );
  if (hf & ETH_RSS_IPV6_EX           ) sh->Printf(",IPV6_EX"           );
  if (hf & ETH_RSS_IPV6_TCP_EX       ) sh->Printf(",IPV6_TCP_EX"       );
  if (hf & ETH_RSS_IPV6_UDP_EX       ) sh->Printf(",IPV6_UDP_EX"       );
  if (hf & ETH_RSS_PORT              ) sh->Printf(",PORT"              );
  if (hf & ETH_RSS_VXLAN             ) sh->Printf(",VXLAN"             );
  if (hf & ETH_RSS_GENEVE            ) sh->Printf(",GENEVE"            );
  if (hf & ETH_RSS_NVGRE             ) sh->Printf(",NVGRE"             );
  sh->Printf("\r\n");
}

