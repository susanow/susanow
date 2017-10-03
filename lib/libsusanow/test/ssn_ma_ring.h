
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

#pragma once
#include <vector>
#include <dpdk/dpdk.h>
#include <slankdev/string.h>

class ssn_ma_ring {
  std::vector<rte_ring*> rings;
 public:
  ssn_ma_ring(size_t n_ring) : rings(n_ring)
  {
    const size_t n_rings = rings.size();
    for (size_t i=0; i<n_rings; i++) {
      auto name = slankdev::format("name%zd", i);
      rings.at(i) = dpdk::ring_alloc(name.c_str(), 1024);
    }
  }
  ~ssn_ma_ring()
  {
    const size_t n_rings = rings.size();
    for (size_t i=0; i<n_rings; i++) {
      auto* r = rings.at(i);
      rte_ring_free(r);
    }
  }
  void debug_dump(FILE* fp) const
  {
    fprintf(fp, "  %-4s  %-10s  %-15s \r\n", "idx", "name", "ptr");
    fprintf(fp, " ------------------------------------------\r\n");
    size_t n_rings = rings.size();
    for (size_t i=0; i<)
  }
}; /* class ssn_ma_ring */


