



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
 * @file thread/pktfwd.h
 * @author slankdev
 */


#pragma once

#include <ssnlib_sys.h>
#include <ssnlib_thread.h>
#include <slankdev/string.h>
#include <slankdev/unused.h>


inline void _pktfwd(System* sys, bool& running, uint8_t port_id, uint8_t queue_id);


class pktfwd : public Fthread {
  System* sys;
  bool running;
  uint8_t port_id;
  uint8_t queue_id;
 public:
  pktfwd(System* s, int pid, int qid)
    : Fthread(
        slankdev::format("pktfwd%u:%u", pid, qid).c_str()
        ), sys(s), running(false), port_id(pid), queue_id(qid) {}
  virtual void impl() override { _pktfwd(sys, running, port_id, queue_id); }
  virtual void kill() override { running = false; }
};



inline void _pktfwd(System* sys, bool& running, uint8_t port_id, uint8_t queue_id)
{
  size_t pid = port_id;
  size_t qid = queue_id;
  running = true;
  while (running) {

      auto& in_port  = sys->ports[pid];
      auto& out_port = sys->ports[pid^1];

      constexpr size_t burst_size = 32;
      rte_mbuf* pkts[burst_size];
      size_t nb_rcv = in_port.rxq[qid].burst(pkts, burst_size);

      out_port.txq[qid].burst(pkts, nb_rcv);

  } /* while */
}

