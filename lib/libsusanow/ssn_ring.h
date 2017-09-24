
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

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <vector>


class rte_ring;

/*
 *            +------+
 * ipacket -> | ring | -> opacket;
 *            +------+
 */
class ssn_ring {
 private:
  rte_ring* ring_;
 public:
  size_t ipacket;
  size_t opacket;
  size_t ipacket_prev;
  size_t opacket_prev;
  size_t ipps;
  size_t opps;
  const char* name() const;

  ssn_ring(const char* name);
  virtual ~ssn_ring();

  int enq(void* obj);
  int deq(void** obj);
  size_t enq_burst(void *const *obj_table, size_t n);
  size_t deq_burst(void**obj_table, size_t n);
};

/*
 * This function must be called once per 1sec
 */
void ssn_ring_getstats_timer_cb(void* arg);
std::vector<ssn_ring*>& ssn_ring_get_rings();


