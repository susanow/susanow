
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
#include <slankdev/vector.h>
#include <slankdev/exception.h>
#define NI(str) slankdev::exception("notimplement "#str);

class accessor {
  size_t accessor_idx;
  std::vector<size_t> ques;
 public:
  accessor() {}
  void set(std::vector<size_t>& vec)
  {
    ques = vec;
    accessor_idx = 0;
  }
  size_t get()
  {
    size_t ret = accessor_idx;
    accessor_idx = (accessor_idx+1) % ques.size();
    return ques[ret];
  }
  size_t get_current() const { return ques[accessor_idx]; }
}; /* class accessor */


class ssn_ma_ring {

  std::vector<rte_ring*> rings;
  std::vector<accessor> accessors;

 public:

  ssn_ma_ring() {}
  ~ssn_ma_ring() { reset_que(); }

  size_t get_num_que() const { return rings.size(); }
  size_t get_num_acc() const { return accessors.size(); }

  void configure_acc(size_t n_acc)
  {
    accessors.resize(n_acc);

    if ((rings.size() % n_acc) != 0) {
      std::string err = "configure_acc: ";
      err += "invalid argument or state ";
      err += slankdev::format("(n_acc=%zd, n_que=%zd)", n_acc, rings.size());
      throw slankdev::exception(err);
    }
    const size_t n_queues_per_accessor = rings.size() / n_acc;

    size_t c=0;
    for (size_t a=0; a<n_acc; a++) {
      std::vector<size_t> vec;
      for (size_t i=0; i<n_queues_per_accessor; i++) {
        vec.push_back(i+c);
      }
      accessors.at(a).set(vec);
      c += n_queues_per_accessor;

      // printf("vec{");
      // for (size_t j=0; j<vec.size(); j++) {
      //   printf("%zd%s", vec[j], j+1<vec.size()?",":"");
      // } printf("}\n");
    }
  }

  void configure_que(size_t n_que)
  {
    rings.resize(n_que);
    const size_t n_rings = n_que;
    for (size_t i=0; i<n_rings; i++) {
      auto name = slankdev::format("name%zd", i);
      rings.at(i) = dpdk::ring_alloc(name.c_str(), 1024);
    }
  }

  void reset_que()
  {
    const size_t n_rings = rings.size();
    for (size_t i=0; i<n_rings; i++) {
      auto* r = rings.at(i);
      rte_ring_free(r);
    }
    rings.resize(0);
  }

  void debug_dump(FILE* fp) const
  {
    fprintf(fp, "\r\n");
    fprintf(fp, " n_queues    : %zd \r\n", rings.size());
    fprintf(fp, " n_accessors : %zd \r\n", accessors.size());
    fprintf(fp, "\r\n");
    fprintf(fp, "  %-4s  %-10s  %-15s  %-5s  %-5s\r\n",
                  "idx", "name", "ptr", "ent", "size");
    fprintf(fp, " ------------------------------------------------------\r\n");
    size_t n_rings = rings.size();
    for (size_t i=0; i<n_rings; i++) {
      rte_ring* ring = rings.at(i);
      fprintf(fp ,"  %-4zd  %-10s  %-15p  %-5d  %-5d\r\n",
          i, ring->name, ring, rte_ring_count(ring), rte_ring_get_size(ring));
    }
    fprintf(fp, "\r\n");
  }

  size_t get_next_qid_from_aid(size_t aid) const { return accessors[aid].get_current(); }
  size_t enqueue_burst(size_t aid, void *const *obj_table, size_t n)
  {
    auto idx = accessors.at(aid).get();
    rte_ring* ring = rings.at(idx);
    size_t ret = rte_ring_enqueue_burst(ring, obj_table, n, nullptr);
    return ret;
  }
  size_t dequeue_burst(void **obj_table, size_t n)
  {
    auto idx = accessors.at(aid).get();
    rte_ring* ring = rings.at(idx);
    throw NI("dequeue_burst");
  }
}; /* class ssn_ma_ring */




