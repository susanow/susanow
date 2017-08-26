
#include <ssn_ring.h>
#include <vector>
#include <dpdk/hdr.h>

std::vector<ssn_ring*> _rings;

const char* ssn_ring::name() const { return ring_->name; }

ssn_ring::ssn_ring(const char* name) : ring_(nullptr)
{
  constexpr size_t socket_id = 0;
  ring_ = rte_ring_create(name, 1024, socket_id, 0);
  _rings.push_back(this);
}

ssn_ring::~ssn_ring()
{
  rte_ring_free(ring_);
  auto nb_rings = _rings.size();
  for (size_t i=0; i<nb_rings; i++) {
    if (_rings[i] == this) {
      _rings.erase(_rings.begin() + i);
      return ;
    }
  }
}

int ssn_ring::enq(void* obj)
{
  int ret = rte_ring_enqueue(ring_, obj);
  // if (ret < 0) return -1;
  ipacket ++ ;
  return ret;
}
int ssn_ring::deq(void** obj)
{
  int ret = rte_ring_dequeue(ring_, obj);
  if (ret < 0) return -1;
  opacket ++;
  return ret;
}
size_t ssn_ring::enq_burst(void *const *obj_table, size_t n)
{
  ipacket += n;
  int ret = rte_ring_enqueue_burst(ring_, obj_table, n, nullptr);
  return ret;
}
size_t ssn_ring::deq_burst(void**obj_table, size_t n)
{
  int ret = rte_ring_dequeue_burst(ring_, obj_table, n, nullptr);
  opacket += ret;
  return ret;
}

void ssn_ring_getstats_timer_cb(void*)
{
  size_t nb_rings = _rings.size();
  for (size_t i=0; i<nb_rings; i++) {
    ssn_ring* sr = _rings[i];
    sr->ipps = sr->ipacket - sr->ipacket_prev;
    sr->opps = sr->opacket - sr->opacket_prev;
    sr->ipacket_prev = sr->ipacket;
    sr->opacket_prev = sr->opacket;
  }
}

std::vector<ssn_ring*>& ssn_ring_get_rings()
{
  return _rings;
}


