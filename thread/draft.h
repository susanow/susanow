
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
 * @file thread/draft.cc
 * @author slankdev
 */

#pragma once

#include <ssnlib_sys.h>
#include <ssnlib_thread.h>
#include <slankdev/string.h>
#include <slankdev/unused.h>

constexpr size_t BURST_SIZE = 32;
size_t d0_begin = 0;
size_t d0_end   = 3;
size_t d1_begin = 0;
size_t d1_end   = 3;
size_t d2_begin = 0;
size_t d2_end   = 3;
size_t d3_begin = 0;
size_t d3_end   = 3;


inline void _draft_pktfwd(System* sys, size_t& que_begin, size_t& que_end)
{
  bool running = true;
  size_t port_in  = 0;
  size_t port_out = 1;
  while (running) {

    for (size_t qid=que_begin; qid<=que_end; qid++) {

      rte_mbuf* pkts[BURST_SIZE];
      size_t nb_rcv = sys->ports[port_in].rxq[qid].burst(pkts, BURST_SIZE);

      rte_delay_us_block(10);

      sys->ports[port_out].txq[qid].burst(pkts, nb_rcv);
    }

  }
}


class draft0 : public Fthread {
  System* sys;
  bool running;
 public:
  draft0(System* s) : Fthread("draft0"), sys(s), running(false) {}
  virtual void kill() override { running = false; }
  virtual void impl() override
  {
    d0_begin = 0;
    d0_end   = 3;
    printf("launch draft0\n");
    _draft_pktfwd(sys, d0_begin, d0_end);
  }
};


class draft1 : public Fthread {
  System* sys;
  bool running;
 public:
  draft1(System* s) : Fthread("draft1"), sys(s), running(false) {}
  virtual void kill() override { running = false; }
  virtual void impl() override
  {
    d0_begin = 0;
    d0_end   = 1;
    d1_begin = 2;
    d1_end   = 3;
    printf("launch draft1\n");
    _draft_pktfwd(sys, d1_begin, d1_end);
  }
};


class draft2 : public Fthread {
  System* sys;
  bool running;
 public:
  draft2(System* s) : Fthread("draft2"), sys(s), running(false) {}
  virtual void kill() override { running = false; }
  virtual void impl() override
  {
    d0_begin = 0;
    d0_end   = 0;
    d1_begin = 1;
    d1_end   = 1;
    d2_begin = 2;
    d2_end   = 3;
    printf("launch draft2\n");
    _draft_pktfwd(sys, d2_begin, d2_end);
  }
};


class draft3 : public Fthread {
  System* sys;
  bool running;
 public:
  draft3(System* s) : Fthread("draft3"), sys(s), running(false) {}
  virtual void kill() override { running = false; }
  virtual void impl() override
  {
    d0_begin = 0;
    d0_end   = 0;
    d1_begin = 1;
    d1_end   = 1;
    d2_begin = 2;
    d2_end   = 2;
    d3_begin = 3;
    d3_end   = 3;
    printf("launch draft3\n");
    _draft_pktfwd(sys, d3_begin, d3_end);
  }
};




