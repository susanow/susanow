
#pragma once

#include <stdint.h>
#include <stddef.h>


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
  size_t enq_bulk(void *const *obj_table, size_t n);
  size_t deq_bulk(void**obj_table, size_t n);
};

/*
 * This function must be called once per 1sec
 */
void ssn_ring_getstats_timer_cb(void* arg);

