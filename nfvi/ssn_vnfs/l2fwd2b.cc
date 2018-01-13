

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

#include <ssn_vnfs/l2fwd2b.h>


void ssn_vnf_l2fwd2b_block_port::deploy_impl(void*)
{
  size_t vlid = get_vlcore_id();
  running = true;
  while (running) {
    rte_mbuf* mbufs[32];
    size_t rxaid = get_lcore_port_rxaid(vlid, port_id);
    size_t n_recv = rx_burst(port_id, rxaid, mbufs, 32);
    if (n_recv == 0) continue;

    for (size_t i=0; i<n_recv; i++) {

      /* Delay Block begin */
      size_t n=10;
      for (size_t j=0; j<100; j++) n++;

    }
    size_t oport_id = get_oportid_from_iportid(port_id);
    size_t txaid = get_lcore_port_txaid(vlid, oport_id);
    size_t n_send = tx_burst(oport_id, txaid, mbufs, n_recv);
    if (n_send < n_recv) {
      dpdk::rte_pktmbuf_free_bulk(&mbufs[n_send], n_recv-n_send);
    }
  }
}

void ssn_vnf_l2fwd2b_block_port::debug_dump(FILE* fp) const
{
  fprintf(fp, " %s \r\n", name.c_str());
  size_t n_lcores = n_vcores();
  for (size_t i=0; i<n_lcores; i++) {
    fprintf(fp, "  vlcore[%zd]: plcore%zd \r\n", i, vcore_id_2_lcore_id(i));
  }
}


