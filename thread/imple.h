
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
 * @file thread/imple.h
 * @author slankdev
 */

#pragma once

#include <ssnlib_sys.h>
#include <ssnlib_thread.h>
#include <slankdev/string.h>



inline void _pcap(System* sys, bool& running)
{
  running = true;
  size_t cnt = 0;
  while (running) {
    size_t nb_ports = sys->ports.size();
    for (uint8_t pid = 0; pid < nb_ports; pid++) {
      uint8_t nb_rxq = sys->ports[pid].rxq.size();
      for (uint8_t qid=0; qid<nb_rxq; qid++) {
        auto& in_port  = sys->ports[pid];

        constexpr size_t bulk_size = 32;
        struct rte_mbuf* pkts[bulk_size];
        size_t nb_rcv = in_port.rxq[qid].burst(pkts, bulk_size);
        for (size_t i=0; i<nb_rcv; i++) {
          printf("%zd: recv len=%u P=%u Q=%u hash=%u\n",
              cnt++,
              rte_pktmbuf_pkt_len(pkts[i]),
              pid, qid, pkts[i]->hash.rss);
          rte_pktmbuf_free(pkts[i]);
        }
      }
    }
  }// while
}


