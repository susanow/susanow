
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
/**
 * @file   example_ssn_port_stat.cc
 * @brief  ssn_port_stat module example
 * @author Hiroki SHIROKURA
 * @date   2017.9.20
 */

#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <ssn_timer.h>
#include <ssn_port_stat.h>
#include <ssn_green_thread.h>
#include <ssn_native_thread.h>
#include <ssn_timer.h>
#include <ssn_port_stat.h>
#include <ssn_cpu.h>
#include <ssn_common.h>
#include <dpdk/dpdk.h>


bool running = true;
void wk(void*)
{
  size_t nb_ports = rte_eth_dev_count();
  while (running) {
    for (size_t p=0; p<nb_ports; p++) {
      rte_mbuf* mbufs[32];
      size_t nb_recv = rte_eth_rx_burst(p, 0, mbufs, 32);
      if (nb_recv == 0) continue;
      size_t nb_send = rte_eth_tx_burst(p^1, 0, mbufs, nb_recv);
      if (nb_recv < nb_send) {
        dpdk::rte_pktmbuf_free_bulk(&mbufs[nb_send], nb_recv-nb_send);
      }
    }
  }
}

void PRINT(void*)
{
  while (running) {
    size_t nb_ports = rte_eth_dev_count();
    printf("  %5s %20s %20s %20s %20s\n",
        "idx", "rx[bps]", "rx[pps]", "tx[bps]", "tx[pps]");
    printf("  ----------------------------------------");
    printf("-----------------------------------------------------\n");
    for (size_t i=0; i<nb_ports; i++) {
      size_t rx_bps = ssn_port_stat_get_cur_rx_pps(i);
      size_t tx_bps = ssn_port_stat_get_cur_tx_pps(i);
      size_t rx_pps = ssn_port_stat_get_cur_rx_bps(i);
      size_t tx_pps = ssn_port_stat_get_cur_tx_bps(i);
      printf("  %5zd %20zd %20zd %20zd %20zd\n", i,
          rx_bps, rx_pps, tx_bps, tx_pps);
    }
    printf("\n\n");
    ssn_sleep(1000);
  }
}

int main(int argc, char** argv)
{
  ssn_init(argc, argv);
  ssn_timer_sched tm1(1);
  ssn_native_thread_launch(ssn_timer_sched_poll_thread, &tm1, 1);

  uint64_t hz = rte_get_timer_hz();
  ssn_timer* tim0 = new ssn_timer(ssn_port_stat_update, nullptr, hz);
  tm1.add(tim0);

  ssn_native_thread_launch(PRINT, nullptr, 3);

  struct rte_eth_conf port_conf;
  dpdk::init_portconf(&port_conf);
  port_conf.rxmode.mq_mode = ETH_MQ_RX_RSS;
  port_conf.rx_adv_conf.rss_conf.rss_key = NULL;
  port_conf.rx_adv_conf.rss_conf.rss_hf = ETH_RSS_IP|ETH_RSS_TCP|ETH_RSS_UDP;
  struct rte_mempool* mp = dpdk::mp_alloc("RXMBUFMP", 0, 8192);

  size_t n_ports = rte_eth_dev_count();
  if (n_ports == 0) throw dpdk::exception("no ethdev");
  printf("%zd ports found \n", n_ports);
  for (size_t i=0; i<n_ports; i++) {
    dpdk::port_configure(i, 1, 1, &port_conf, mp);
  }


  ssn_native_thread_launch(wk, nullptr, 2);
  sleep(5);
  running = false;
  tm1.del(tim0); delete (tim0);
  ssn_fin();
}


