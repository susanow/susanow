
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <rte_ring.h>


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
  const char* name() const { return ring_->name; }

  ssn_ring(const char* name) : ring_(nullptr)
  { ring_ = slankdev::ring_alloc(name, 1024); }
  virtual ~ssn_ring() { rte_ring_free(ring_); }

  int enq(void* obj)
  {
    int ret = rte_ring_enqueue(ring_, obj);
    // if (ret < 0) return -1;
    ipacket ++ ;
    return ret;
  }
  int deq(void** obj)
  {
    int ret = rte_ring_dequeue(ring_, obj);
    if (ret < 0) return -1;
    opacket ++;
    return ret;
  }
  size_t enq_bulk(void *const *obj_table, size_t n)
  {
    ipacket += n;
    int ret = rte_ring_enqueue_bulk(ring_, obj_table, n, nullptr);
    return ret;
  }
  size_t deq_bulk(void**obj_table, size_t n)
  {
    int ret = rte_ring_dequeue_bulk(ring_, obj_table, n, nullptr);
    opacket += ret;
    return ret;
  }
};

/*
 * This function must be called once per 1sec
 */
inline void ssn_ring_getstats_timer_cb(void* arg)
{
  ssn_ring* sr = reinterpret_cast<ssn_ring*>(arg);
  sr->ipps = sr->ipacket - sr->ipacket_prev;
  sr->opps = sr->opacket - sr->opacket_prev;
  sr->ipacket_prev = sr->ipacket;
  sr->opacket_prev = sr->opacket;
}


