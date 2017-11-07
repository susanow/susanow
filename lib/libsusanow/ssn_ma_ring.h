
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


class ssn_ma_ring {

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

  std::vector<rte_ring*> rings;
  std::vector<accessor> enq_accessors;
  std::vector<accessor> deq_accessors;
  std::string name_prefix;

  size_t prod_pps_sum;
  size_t prod_pps_cur;
  size_t cons_pps_sum;
  size_t cons_pps_cur;

 public:

  ssn_ma_ring(const char* np)
    : name_prefix(np), prod_pps_sum(0), cons_pps_cur(0) {}
  ~ssn_ma_ring() { reset_que(); }

  size_t get_num_que() const { return rings.size(); }
  size_t get_num_enq_acc() const { return enq_accessors.size(); }
  size_t get_num_deq_acc() const { return deq_accessors.size(); }

  /**
   * @brief update ring-statistics for timer-function.
   * @details
   *   This function must be called once a second.
   */
  void update_stats()
  {
    prod_pps_cur = prod_pps_sum;
    prod_pps_sum = 0;
    cons_pps_cur = cons_pps_sum;
    cons_pps_sum = 0;
  }
  size_t get_prod_perf() const { return prod_pps_cur; }
  size_t get_cons_perf() const { return cons_pps_cur; }

  void configure_acc(size_t n_enq_acc, size_t n_deq_acc)
  {
    configure_deq_acc(n_deq_acc);
    configure_enq_acc(n_enq_acc);
  }

  void configure_deq_acc(size_t n_deq_acc)
  {
    deq_accessors.resize(n_deq_acc);

    if ((rings.size() % n_deq_acc) != 0) {
      std::string err = "configure_acc: ";
      err += "invalid deq argument or state ";
      err += slankdev::format("(n_deq_acc=%zd, n_que=%zd)", n_deq_acc, rings.size());
      throw slankdev::exception(err);
    }

    /*
     * deq configuration
     */
    const size_t n_deq_queues_per_accessor = rings.size() / n_deq_acc;
    size_t deq_c=0;
    for (size_t a=0; a<n_deq_acc; a++) {
      std::vector<size_t> vec;
      for (size_t i=0; i<n_deq_queues_per_accessor; i++) {
        vec.push_back(i+deq_c);
      }
      deq_accessors.at(a).set(vec);
      deq_c += n_deq_queues_per_accessor;

      // printf("vec{");
      // for (size_t j=0; j<vec.size(); j++) {
      //   printf("%zd%s", vec[j], j+1<vec.size()?",":"");
      // } printf("}\n");
    }
  }

  void configure_enq_acc(size_t n_enq_acc)
  {
    enq_accessors.resize(n_enq_acc);

    if ((rings.size() % n_enq_acc) != 0) {
      std::string err = "configure_acc: ";
      err += "invalid enq argument or state ";
      err += slankdev::format("(n_enq_acc=%zd, n_que=%zd)", n_enq_acc, rings.size());
      throw slankdev::exception(err);
    }

    /*
     * enq configuration
     */
    const size_t n_enq_queues_per_accessor = rings.size() / n_enq_acc;
    size_t enq_c=0;
    for (size_t a=0; a<n_enq_acc; a++) {
      std::vector<size_t> vec;
      for (size_t i=0; i<n_enq_queues_per_accessor; i++) {
        vec.push_back(i+enq_c);
      }
      enq_accessors.at(a).set(vec);
      enq_c += n_enq_queues_per_accessor;

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
      auto name = slankdev::format("%s-ring%zd", name_prefix.c_str(), i);
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
    fprintf(fp, " n_queues        : %zd \r\n", rings.size());
    fprintf(fp, " n_enq_accessors : %zd \r\n", enq_accessors.size());
    fprintf(fp, " n_deq_accessors : %zd \r\n", deq_accessors.size());
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

  size_t get_next_enqid_from_aid(size_t aid) const { return enq_accessors[aid].get_current(); }
  size_t get_next_deqid_from_aid(size_t aid) const { return deq_accessors[aid].get_current(); }

  size_t enqueue_burst(size_t aid, void *const *obj_table, size_t n)
  {
    auto idx = enq_accessors[aid].get();
    rte_ring* ring = rings[idx];
    size_t ret = rte_ring_enqueue_burst(ring, obj_table, n, nullptr);
    prod_pps_sum += ret;
    return ret;
  }

  size_t dequeue_burst(size_t aid, void **obj_table, size_t n)
  {
    auto idx = deq_accessors[aid].get();
    rte_ring* ring = rings[idx];
    size_t ret = rte_ring_dequeue_burst(ring, obj_table, n, nullptr);
    cons_pps_sum += ret;
    return ret;
  }
}; /* class ssn_ma_ring */




