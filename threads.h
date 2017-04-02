



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
 * @file thread.h
 * @author slankdev
 */


#pragma once

#include <ssnlib_sys.h>
#include <ssnlib_thread.h>
#include <slankdev/string.h>
#include <slankdev/extra/dpdk_header.h>




class timertest : public Tthread {
  System* sys;
 public:
  timertest(System* s) : Tthread("timertest"), sys(s) {}
  void impl()
  {
    for (Port& port : sys->ports) {
      port.stats.update();
      port.link.update();
    }
  }
};



class lthread_test : public Lthread {
 public:
  int a;
  lthread_test(int b)
    : Lthread(slankdev::fs("lthread_test(%d)", b).c_str()),
    a(b) {}
  virtual void impl() override {
    printf("lthread_test(%d)\n", a);
    lthread_sleep(1 * 1000 * 1000 * 1000);
  }
  virtual void kill() override {
    running = false;
  }
};



class fthread_test : public Fthread {
  int a;
  bool running;
 public:
  fthread_test(int b)
    : Fthread(slankdev::fs("fthread_test(%d)", b).c_str()),
    a(b),
    running(false) {}
  virtual void impl() override {
    running = true;
    while (running) {
      printf("fthread_test(%d)\n", a);;
      sleep(1);
    }
  }
  virtual void kill() override { running = false; }
};




class pcap : public Fthread {
  System* sys;
  bool running;
 public:
  pcap(System* s) : Fthread("pcap"), sys(s), running(false) {}
  virtual void impl() override
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
            printf("%zd: recv len=%u P=%u Q=%u \n",
                cnt++,
                rte_pktmbuf_pkt_len(pkts[i]),
                pid, qid);
            rte_pktmbuf_free(pkts[i]);
          }
        }
      }
    }// while
  }
  virtual void kill() override { running = false; }
};



#if 1
class txrxwk : public Fthread {
  System* sys;
  bool running;
 public:
  txrxwk(System* s) : Fthread("txrxwk"), sys(s), running(false) {}
  void impl()
  {

    running = true;
    size_t nb_ports = sys->ports.size();
    while (running) {
      for (uint8_t pid = 0; pid < nb_ports; pid++) {
        uint8_t nb_rxq = sys->ports[pid].rxq.size();
        uint8_t nb_txq = sys->ports[pid].txq.size();
        assert(nb_txq == nb_rxq);

        for (uint8_t qid=0; qid<nb_rxq; qid++) {
          auto& in_port  = sys->ports[pid];
          auto& out_port = sys->ports[pid^1];

          constexpr size_t burst_size = 32;
          rte_mbuf* pkts[burst_size];
          size_t nb_rcv = in_port.rxq[qid].burst(pkts, burst_size);
          out_port.txq[qid].burst(pkts, nb_rcv);
        }
      }
    }
  }
  void kill() override { running = false; }
};
#endif


