
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
  size_t cur_rx_mis;
  size_t cur_rx_nombuf;

  rte_eth_stats cur;
  rte_eth_stats prev;

  ssn_port_stat(size_t i) : lcore_id(i),
    cur_rx_pps(0), cur_tx_pps(0),
    cur_rx_bps(0), cur_tx_bps(0),
    cur_rx_err(0), cur_tx_err(0),
    cur_rx_mis(0),
    cur_rx_nombuf(0)
  {
    memset(&cur, 0x0, sizeof(rte_eth_stats));
    memset(&prev, 0x0, sizeof(rte_eth_stats));
  }
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
    cur_rx_mis = cur.imissed - prev.imissed;
    cur_rx_nombuf = cur.rx_nombuf - prev.rx_nombuf;
  }
  void debug_dump(FILE* fp) const
  {
    fprintf(fp, "Port%zd    \r\n", lcore_id);
    fprintf(fp, "  rx/tx[pps]: %zd/%zd    \r\n", cur_rx_pps, cur_tx_pps);
    fprintf(fp, "  rx/tx[bps]: %zd/%zd    \r\n", cur_rx_bps, cur_tx_bps);
    fprintf(fp, "  rx/tx[err]: %zd/%zd    \r\n", cur.ierrors, cur.oerrors);
    fprintf(fp, "  rxmiss[pps]: %zd \r\n", cur_rx_mis);
    fprintf(fp, "  rx nombuf: %zd  \r\n", cur.rx_nombuf);
  }
};
ssn_port_stat* portstat[RTE_MAX_ETHPORTS];

void ssn_port_stat_update(void*)
{
  for (size_t i=0; i<RTE_MAX_ETHPORTS; i++) {
    if (portstat[i])
      portstat[i]->update();
  }
}

void ssn_port_stat_debug_dump(FILE* fp, size_t port_id)
{
  portstat[port_id]->debug_dump(fp);
}

void ssn_port_stat_init()
{
  size_t nb_ports = rte_eth_dev_count();
  for (size_t i=0; i<nb_ports; i++) {
    portstat[i] = new ssn_port_stat(i);
  }
}

void ssn_port_stat_init_pid(size_t pid)
{
  if (!portstat[pid])
    portstat[pid] = new ssn_port_stat(pid);
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

size_t ssn_port_stat_get_cur_rx_mis(size_t pid) { return portstat[pid]->cur_rx_mis; }
size_t ssn_port_stat_get_cur_rx_pps(size_t pid) { return portstat[pid]->cur_rx_pps; }
size_t ssn_port_stat_get_cur_tx_pps(size_t pid) { return portstat[pid]->cur_tx_pps; }
size_t ssn_port_stat_get_cur_rx_bps(size_t pid) { return portstat[pid]->cur_rx_bps; }
size_t ssn_port_stat_get_cur_tx_bps(size_t pid) { return portstat[pid]->cur_tx_bps; }


