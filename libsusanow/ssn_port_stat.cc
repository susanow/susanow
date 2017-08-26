
#include <dpdk/hdr.h>
#include <ssn_port_stat.h>

class ssn_port_stat {
  const size_t lcore_id;
 public:
  size_t cur_rx_pps;
  size_t cur_tx_pps;
  size_t cur_rx_bps;
  size_t cur_tx_bps;
  size_t cur_rx_err;
  size_t cur_tx_err;
  size_t cur_rx_nombuf;

  rte_eth_stats cur;
  rte_eth_stats prev;

  ssn_port_stat(size_t i) : lcore_id(i),
    cur_rx_pps(0), cur_tx_pps(0),
    cur_rx_bps(0), cur_tx_bps(0),
    cur_rx_err(0), cur_tx_err(0),
    cur_rx_nombuf(0) {}
  void update()
  {
    prev = cur;
    rte_eth_stats_get(lcore_id, &cur);
    cur_rx_pps = cur.ipackets - prev.ipackets;
    cur_tx_pps = cur.opackets - prev.opackets;
    cur_rx_bps = (cur.ibytes - prev.ibytes + cur_rx_pps*24) << 3;
    cur_tx_bps = (cur.obytes - prev.obytes + cur_tx_pps*24) << 3;
    cur_rx_err = cur.ierrors - prev.ierrors;
    cur_tx_err = cur.oerrors - prev.oerrors;
    cur_rx_nombuf = cur.rx_nombuf - prev.rx_nombuf;
  }
  void dump() const // ERASE
  {
    printf("Port%zd    ", lcore_id);
    printf("  rx/tx[pps]: %zd/%zd    ", cur_rx_pps, cur_tx_pps);
    printf("  rx/tx[bps]: %zd/%zd    ", cur_rx_bps, cur_tx_bps);
    printf("  rx/tx[err]: %zd/%zd    ", cur.ierrors, cur.oerrors);
    printf("  rx nombuf: %zd  \n", cur.rx_nombuf);
  }
};
ssn_port_stat* portstat[RTE_MAX_ETHPORTS];

void ssn_port_stat_update(void*)
{
  size_t nb_ports = rte_eth_dev_count();
  for (size_t i=0; i<nb_ports; i++) {
    portstat[i]->update();
  }
}

void ssn_port_stat_init()
{
  size_t nb_ports = rte_eth_dev_count();
  for (size_t i=0; i<nb_ports; i++) {
    portstat[i] = new ssn_port_stat(i);
  }
}

void ssn_port_stat_fin()
{
  size_t nb_ports = rte_eth_dev_count();
  for (size_t i=0; i<nb_ports; i++) {
    delete portstat[i];
  }
}

size_t ssn_port_stat_get_tot_rx_packets(size_t pid) { return portstat[pid]->cur.ipackets; }
size_t ssn_port_stat_get_tot_tx_packets(size_t pid) { return portstat[pid]->cur.opackets; }
size_t ssn_port_stat_get_tot_rx_bytes(size_t pid) { return portstat[pid]->cur.ibytes; }
size_t ssn_port_stat_get_tot_tx_bytes(size_t pid) { return portstat[pid]->cur.obytes; }

size_t ssn_port_stat_get_cur_rx_pps(size_t pid) { return portstat[pid]->cur_rx_pps; }
size_t ssn_port_stat_get_cur_tx_pps(size_t pid) { return portstat[pid]->cur_tx_pps; }
size_t ssn_port_stat_get_cur_rx_bps(size_t pid) { return portstat[pid]->cur_rx_bps; }
size_t ssn_port_stat_get_cur_tx_bps(size_t pid) { return portstat[pid]->cur_tx_bps; }


