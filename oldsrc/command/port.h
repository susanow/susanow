

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
 * @file command/port.h
 * @author slankdev
 */

#pragma once

#include <ssnlib_thread.h>
#include <ssnlib_misc.h>
#include <command/thread.h>
#include <command/imple.h>
#include <command/node.h>

#include <slankdev/vty.h>
#include <slankdev/unused.h>
#include <slankdev/exception.h>
#include <slankdev/extra/dpdk_struct.h>

#include <rte_eth_ctrl.h>
#include <rte_flow.h>





class port_statistics_reset : public slankdev::command {
 public:
  port_statistics_reset()
  {
    nodes.push_back(fixed_port());
    nodes.push_back(new slankdev::node_fixedstring("statistics",
          "show statistics data"));
    nodes.push_back(new slankdev::node_fixedstring("reset",
          "reset statistics data"));
  }
  virtual void func(slankdev::shell* sh) override
  { _port_statistics_reset(sh); }
};

class port_statistics : public slankdev::command {
 public:
  port_statistics()
  {
    nodes.push_back(fixed_port());
    nodes.push_back(new slankdev::node_fixedstring("statistics",
          "show statistics data"));
  }
  virtual void func(slankdev::shell* sh) override { _port_statistics(sh); }
};


// TODO: ERASE
class port_rxmode_show : public slankdev::command {
 public:
  port_rxmode_show()
  {
    nodes.push_back(fixed_port());
    nodes.push_back(new slankdev::node_fixedstring("rxmode", ""));
    nodes.push_back(fixed_show());
  }
  virtual void func(slankdev::shell* sh) override { _port_rxmode_show(sh); }
};



class port_rss : public slankdev::command {
 public:
  port_rss()
  {
    nodes.push_back(fixed_port());
    nodes.push_back(new slankdev::node_fixedstring("rss", ""));
  }
  virtual void func(slankdev::shell* sh) override { _port_rss(sh); }
};



class port_dev_start : public slankdev::command {
 public:
  port_dev_start()
  {
    nodes.push_back(fixed_port());
    nodes.push_back(fixed_dev());
    nodes.push_back(new slankdev::node_fixedstring("start", ""));
  }
  virtual void func(slankdev::shell* sh) override
  {
    System* sys = get_sys(sh);
    size_t nb_ports = sys->ports.size();
    for (size_t i=0; i<nb_ports; i++)
      sys->ports[i].devstart();
  }
};
class port_dev_stop : public slankdev::command {
 public:
  port_dev_stop()
  {
    nodes.push_back(fixed_port());
    nodes.push_back(fixed_dev());
    nodes.push_back(new slankdev::node_fixedstring("stop", ""));
  }
  virtual void func(slankdev::shell* sh) override
  {
    System* sys = get_sys(sh);
    size_t nb_ports = sys->ports.size();
    for (size_t i=0; i<nb_ports; i++)
      sys->ports[i].devstop();
  }
};

class port_link_down : public slankdev::command {
 public:
  port_link_down()
  {
    nodes.push_back(fixed_port());
    nodes.push_back(fixed_link());
    nodes.push_back(new slankdev::node_fixedstring("down", ""));
  }
  virtual void func(slankdev::shell* sh) override
  {
    System* sys = get_sys(sh);
    size_t nb_ports = sys->ports.size();
    for (size_t i=0; i<nb_ports; i++)
      sys->ports[i].linkdown();
  }
};

class port_link_up : public slankdev::command {
 public:
  port_link_up()
  {
    nodes.push_back(fixed_port());
    nodes.push_back(fixed_link());
    nodes.push_back(new slankdev::node_fixedstring("up", ""));
  }
  virtual void func(slankdev::shell* sh) override
  {
    System* sys = get_sys(sh);
    size_t nb_ports = sys->ports.size();
    for (size_t i=0; i<nb_ports; i++)
      sys->ports[i].linkup();
  }
};


class port_set_nbq : public slankdev::command {
 public:
  port_set_nbq()
  {
    nodes.push_back(fixed_port());
    nodes.push_back(new slankdev::node_fixedstring("set", ""));
    nodes.push_back(new slankdev::node_fixedstring("nbq", ""));
    nodes.push_back(new slankdev::node_string              );
  }
  virtual void func(slankdev::shell* sh) override
  {
    std::string s = nodes[3]->get();
    sh->Printf("set nb rings %s \r\n", s.c_str());
    try {
      size_t n = std::stoi(s);

      System* sys = get_sys(sh);
      size_t nb_ports = sys->ports.size();
      for (size_t i=0; i<nb_ports; i++) {
        sys->ports[i].nb_rx_rings = n;
        sys->ports[i].nb_tx_rings = n;
      }
    } catch (std::exception& e) {
      sh->Printf("Invalid Syntax\r\n");
    }
  }
};



class port_configure : public slankdev::command {
 public:
  port_configure()
  {
    nodes.push_back(fixed_port());
    nodes.push_back(new slankdev::node_fixedstring("configure", ""));
  }
  virtual void func(slankdev::shell* sh) override
  {
    System* sys = get_sys(sh);
    size_t nb_ports = sys->ports.size();

#if 0 // TEST
    for (size_t i=0; i<nb_ports; i++) {
      csrc_set_rte_fdir_conf(&sys->ports[i].conf.raw.fdir_conf);
    }
#endif

    for (size_t i=0; i<nb_ports; i++) {
      sys->ports[i].init();
    }
  }
};



inline void _port_show_conf(slankdev::shell* sh)
{
  System* sys = get_sys(sh);
  if (sys->ports.empty()) {
    sh->Printf("Port not found :( \r\n");
    return ;
  }

  /*
   * Flow Director Configuration
   */
  const rte_fdir_conf& fdir_conf = sys->ports[0].conf.raw.fdir_conf;
  sh->Printf(" fdir_conf { \r\n");
  sh->Printf("   mode:    %s \r\n", slankdev::rte_fdir_mode2str(fdir_conf.mode));
  sh->Printf("   pballoc: %s \r\n", slankdev::rte_fdir_pballoc_type2str(fdir_conf.pballoc));
  sh->Printf("   status:  %s \r\n", slankdev::rte_fdir_status_mode2str(fdir_conf.status));
  sh->Printf("   mask  = {\r\n");
  sh->Printf("     .vlan_tci_mask     = %u \r\n", fdir_conf.mask.vlan_tci_mask);
  sh->Printf("     .ipv4_mask         = { \r\n");
  sh->Printf("         src  :%x  \r\n", fdir_conf.mask.ipv4_mask.src_ip);
  sh->Printf("         dst  :%x  \r\n", fdir_conf.mask.ipv4_mask.dst_ip);
  sh->Printf("         top  :%u  \r\n", fdir_conf.mask.ipv4_mask.tos);
  sh->Printf("         ttl  :%u  \r\n", fdir_conf.mask.ipv4_mask.ttl);
  sh->Printf("         proto:%u  \r\n", fdir_conf.mask.ipv4_mask.proto);
  sh->Printf("     } \r\n");
  sh->Printf("     .ipv6_mask = { \r\n");
  sh->Printf("         src = %x:%x:%x:%x \r\n",
      fdir_conf.mask.ipv6_mask.src_ip[0], fdir_conf.mask.ipv6_mask.src_ip[1],
      fdir_conf.mask.ipv6_mask.src_ip[2], fdir_conf.mask.ipv6_mask.src_ip[3]);
  sh->Printf("         dst = %x:%x:%x:%x \r\n",
      fdir_conf.mask.ipv6_mask.dst_ip[0], fdir_conf.mask.ipv6_mask.dst_ip[1],
      fdir_conf.mask.ipv6_mask.dst_ip[2], fdir_conf.mask.ipv6_mask.dst_ip[3]);
  sh->Printf("         tc  = %u \r\n"         , fdir_conf.mask.ipv6_mask.tc);
  sh->Printf("         proto = %u      \r\n"  , fdir_conf.mask.ipv6_mask.proto);
  sh->Printf("         hop_limits = %u \r\n"  , fdir_conf.mask.ipv6_mask.hop_limits);
  sh->Printf("     } \r\n");
  sh->Printf("     .src_port_mask      = %u \r\n", fdir_conf.mask.src_port_mask     );
  sh->Printf("     .dst_port_mask      = %u \r\n", fdir_conf.mask.dst_port_mask     );
  sh->Printf("     .mac_addr_byte_mask = %u \r\n", fdir_conf.mask.mac_addr_byte_mask);
  sh->Printf("     .tunnel_id_mask     = %u \r\n", fdir_conf.mask.tunnel_id_mask    );
  sh->Printf("     .tunnel_type_mask   = %u \r\n", fdir_conf.mask.tunnel_type_mask  );
  sh->Printf("   } \r\n");
  sh->Printf("   drop_queue = %u      \r\n", fdir_conf.drop_queue);
  sh->Printf("   flex_conf { \r\n");
  sh->Printf("     nb_payloads  = %u  \r\n", fdir_conf.flex_conf.nb_payloads);
  sh->Printf("     nb_flexmasks = %u  \r\n", fdir_conf.flex_conf.nb_flexmasks);

#if 1
  sh->Printf("     flex_set : TODO\r\n");
  sh->Printf("     flex_mask: TODO\r\n");
#else
  sh->Printf("     flex_set: \r\n");
  for (size_t i=0; i<RTE_ETH_PAYLOAD_MAX; i++) {
    sh->Printf("       [%2d]: \r\n", i);
    sh->Printf("         type: %s \r\n",
        slankdev::rte_eth_payload_type2str(fdir_conf.flex_conf.flex_set[i].type));
    sh->Printf("       src_offset:\r\n");
    for (size_t j=0; j<RTE_ETH_FDIR_MAX_FLEXLEN; j++) {
      sh->Printf("%u, ",
          fdir_conf.flex_conf.flex_set[i].src_offset[j]);
    }
    sh->Printf("\r\n");
  }
  sh->Printf("     flex_mask   : TODO\r\n");
  for (size_t i=0; i<RTE_ETH_FLOW_MAX; i++) {
    sh->Printf("       [%2d]:  \r\n", i);
  }
#endif

  sh->Printf("   } \r\n");
  sh->Printf(" }\r\n");

  /*
   * RxMode Configuration
   */
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


class port_show_conf : public slankdev::command {
 public:
  port_show_conf()
  {
    nodes.push_back(fixed_port());
    nodes.push_back(new slankdev::node_fixedstring("show", ""));
    nodes.push_back(new slankdev::node_fixedstring("conf", ""));
  }
  virtual void func(slankdev::shell* sh) override { _port_show_conf(sh); }
};





