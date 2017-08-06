
#pragma once
#include <ssn_ring.h>
#include <slankdev/extra/dpdk.h>

/*
 * Must be accessed by single producer & single consumer
 * rings_[producer][consumer]
 */
class ssn_rr_rx_rings {
  std::vector<ssn_ring*> rings;
  size_t pivot;
  size_t size;
 public:
  ssn_rr_rx_rings() : pivot(0) {}
  virtual ~ssn_rr_rx_rings() {}
  void add_ring(ssn_ring* ring) { rings.push_back(ring); size++; }
  void del_ring(ssn_ring* ring)
  {
    for (size_t i=0; i<size; i++) {
      if (rings[i] == ring) {
        rings.erase(rings.begin() + i);
        size --;
        return ;
      }
    }
    throw slankdev::exception("not found");
  }
  size_t get_size() const { return size; }

  int enq(rte_mbuf* mbuf)
  {
    ssn_ring* ring = rings[pivot];
    size_t size = get_size();
    int ret = ring->enq((void*)mbuf);
    pivot ++;
    pivot = pivot % size;
    return ret;
  }
  size_t enq_burst(rte_mbuf* mbufs, size_t nb_mbufs)
  {
    ssn_ring* ring = rings[pivot];
    size_t size = get_size();
    size_t nb_enq = ring->enq_burst((void**)mbufs, nb_mbufs);
    pivot ++;
    pivot = pivot % size;
    return nb_enq;
  }
  int enq(rte_mbuf** mbuf)
  {
    ssn_ring* ring = rings[pivot];
    size_t size = get_size();
    int ret = ring->deq((void**)mbuf);
    pivot ++;
    pivot = pivot % size;
    return ret;
  }
  size_t deq_burst(rte_mbuf* mbufs, size_t nb_mbufs)
  {
    ssn_ring* ring = rings[pivot];
    size_t size = get_size();
    size_t nb_deq = ring->deq_burst((void**)mbufs, nb_mbufs);
    pivot ++;
    pivot = pivot % size;
    return nb_deq;
  }
};



// void dummy()
// {
//   rx_rings[] = {rx0, rx1};
//   while (true) {
//     for (i=0; i<nb_rx_rings; i++) {
//       nb_deq = rx_rings[i]->deq_burst(mbufs, 32);
//       tx_rings[i]->enq_burst(mbufs, nb_deq);
//     }
//   }
// }


