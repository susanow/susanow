
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

#include <ssn_vnfs/l2fwd1b_delay.h>


void ssn_vnf_l2fwd1b_delay_block::deploy_impl(void*)
{
  size_t lcore_id = ssn_lcore_id();
  size_t vcore_id  = get_vlcore_id();

  running = true;
  while (running) {
    size_t n_port = this->n_ports();
    for (size_t pid=0; pid<n_port; pid++) {

      rte_mbuf* mbufs[32];
      size_t rxaid = get_lcore_port_rxaid(vcore_id, pid);
      size_t txaid = get_lcore_port_txaid(vcore_id, pid^1);

      size_t n_recv = rx_burst(pid, rxaid, mbufs, 32);
      if (n_recv == 0) continue;

      for (size_t i=0; i<n_recv; i++) {

        /* Delay Block begin */
        size_t n=10;
        for (size_t j=0; j<50; j++) n++;

      }
      size_t n_send = tx_burst(pid^1, txaid, mbufs, n_recv);
      if (n_send < n_recv) {
        dpdk::rte_pktmbuf_free_bulk(&mbufs[n_send], n_recv-n_send);
      }

    } /* for */
  } /* while (running) */
}


