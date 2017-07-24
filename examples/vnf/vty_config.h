
#pragma once
#include <ssn_vty.h>

inline vty_cmd_match vtymt_slank()
{
  vty_cmd_match m;
  m.nodes.push_back(new node_fixedstring("slank", ""));
  return m;
}
inline void vtycb_slank(vty_cmd_match* m, vty_client* sh, void*)
{
  static size_t cnt = 0;
  sh->Printf("slankdev called %zd times\r\n", cnt++);
}
// vty_cmd_match vtymt_show_vnf()
// {
//   vty_cmd_match m;
//   m.nodes.push_back(new node_fixedstring("show", ""));
//   m.nodes.push_back(new node_fixedstring("vnf", ""));
//   return m;
// }
// inline void vtycb_show_vnf(vty_cmd_match* m, vty_client* sh, void* arg)
// {
//   vnf* vnf0 = *reinterpret_cast<vnf**>(arg);
//   FILE* fp = fdopen(sh->get_fd(), "w");
//   vnf0->debug_dump(fp);
//   fflush(fp);
// }
// inline vty_cmd_match vtymt_vnfctl()
// {
//   vty_cmd_match m;
//   m.nodes.push_back(new node_fixedstring("vnfctl", ""));
//   return m;
// }
// inline vty_cmd_match vtymt_vnfctl_STR_inc()
// {
//   vty_cmd_match m;
//   m.nodes.push_back(new node_fixedstring("vnfctl", ""));
//   m.nodes.push_back(new node_string                   );
//   m.nodes.push_back(new node_fixedstring("inc", "increment thread muximize"));
//   return m;
// }
// inline void vtycb_vnfctl_STR_inc(vty_cmd_match* m, vty_client* sh, void* arg)
// {
//   std::vector<vnf*>& vnfs = *reinterpret_cast<std::vector<vnf*>*>(arg);
//   auto nb_vnfs = vnfs.size();
//   std::string s = m->nodes[1]->get();
//   for (auto i=0; i<nb_vnfs; i++) {
//     if (s == vnfs[i]->name) {
//       vnfs[i]->pl[0].inc();
//       return ;
//     }
//   }
// }

inline vty_cmd_match vtymt_show_port()
{
  vty_cmd_match m;
  m.nodes.push_back(new node_fixedstring("show", ""));
  m.nodes.push_back(new node_fixedstring("port", ""));
  return m;
}
inline void vtycb_show_port(vty_cmd_match* m, vty_client* sh, void* arg)
{
  size_t nb_ports = ssn_dev_count();
  for (size_t i=0; i<nb_ports; i++) {
    size_t rx_pps = ssn_port_stat_get_cur_rx_pps(i);
    size_t tx_pps = ssn_port_stat_get_cur_tx_pps(i);
    size_t rx_bps = ssn_port_stat_get_cur_rx_bps(i);
    size_t tx_bps = ssn_port_stat_get_cur_tx_bps(i);
    sh->Printf("Port[%zd]\r\n", i);
    sh->Printf(" rx_pps: %zd [pps]\r\n", rx_pps);
    sh->Printf(" tx_pps: %zd [pps]\r\n", tx_pps);
    sh->Printf(" rx_bps: %zd [bps]\r\n", rx_bps);
    sh->Printf(" tx_bps: %zd [bps]\r\n", tx_bps);

    struct rte_eth_link link;
    rte_eth_link_get_nowait(i, &link);
    sh->Printf(" link  : %s-", link.link_status==ETH_LINK_DOWN?"DOWN":"UP");
    switch (link.link_speed) {
      case ETH_SPEED_NUM_NONE: sh->Printf("NONE"); break;
      case ETH_SPEED_NUM_10M : sh->Printf("10M" ); break;
      case ETH_SPEED_NUM_100M: sh->Printf("100M"); break;
      case ETH_SPEED_NUM_1G  : sh->Printf("1G"  ); break;
      case ETH_SPEED_NUM_2_5G: sh->Printf("2.5G"); break;
      case ETH_SPEED_NUM_5G  : sh->Printf("5G"  ); break;
      case ETH_SPEED_NUM_10G : sh->Printf("10G" ); break;
      case ETH_SPEED_NUM_20G : sh->Printf("20G" ); break;
      case ETH_SPEED_NUM_25G : sh->Printf("25G" ); break;
      case ETH_SPEED_NUM_40G : sh->Printf("40G" ); break;
      case ETH_SPEED_NUM_50G : sh->Printf("50G" ); break;
      case ETH_SPEED_NUM_56G : sh->Printf("56G" ); break;
      case ETH_SPEED_NUM_100G: sh->Printf("100G"); break;
      default: throw slankdev::exception("unknown link speed");
    }
    sh->Printf("-%s\r\n", link.link_duplex==ETH_LINK_HALF_DUPLEX?"HD":"FD");
  }
}
inline vty_cmd_match vtymt_show_ring()
{
  vty_cmd_match m;
  m.nodes.push_back(new node_fixedstring("show", ""));
  m.nodes.push_back(new node_fixedstring("ring", ""));
  return m;
}
inline void vtycb_show_ring(vty_cmd_match* m, vty_client* sh, void* arg)
{
  std::vector<ssn_ring*>& rings = ssn_ring_get_rings();
  size_t nb_rings = rings.size();
  for (size_t i=0; i<nb_rings; i++) {
    size_t ipps = rings[i]->ipps;
    size_t opps = rings[i]->opps;
    sh->Printf("ring[%zd]\r\n", i);
    sh->Printf(" name  : %s \r\n", rings[i]->name());
    sh->Printf(" rx_pps: %zd [pps]\r\n", ipps);
    sh->Printf(" tx_pps: %zd [pps]\r\n", opps);
  }
}


