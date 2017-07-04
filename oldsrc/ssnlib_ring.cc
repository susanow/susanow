

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
 * @file ssnlib_ring.h
 * @brief wrap rte_ring.h
 * @author slankdev
 */


#include <ssnlib_ring.h>

#include <rte_ring.h>
#include <rte_config.h>
#include <rte_version.h>
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_ether.h>
#include <rte_cycles.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>
#include <rte_hexdump.h>
#include <rte_ip.h>
#include <rte_ip_frag.h>

#include <slankdev/exception.h>

#include <ssnlib_log.h>


static inline void __attribute__((always_inline))
rte_pktmbuf_free_bulk(struct rte_mbuf *m_list[], int16_t npkts)
{
  while (npkts--) {
    rte_pktmbuf_free(*m_list++);
  }
}



#if 0
void Ring_stdqueue::push_bulk(rte_mbuf** obj_table, size_t n)
{
  auto_lock lg(*m);
  for (size_t i=0; i<n; i++) {
    queue_.push(obj_table[i]);
  }
}
bool Ring_stdqueue::pop_bulk(rte_mbuf** obj_table, size_t n)
{
  auto_lock lg(*m);
  if (queue_.size() < n)
    return false;

  for (size_t i=0; i<n; i++) {
    obj_table[i] = queue_.front();
    queue_.pop();
  }
  return true;
}








Ring_dpdk::Ring_dpdk(size_t count, size_t p, size_t q, const char* nameprefix)
  : ring_depth(count)
{
  std::string rn = nameprefix + std::to_string(p) + ":" + std::to_string(q);
  uint16_t socket_id = rte_socket_id();

  ring_ = rte_ring_create(rn.c_str(), count, socket_id, 0);
  if (!ring_) {
    char errstr[256];
    snprintf(errstr, sizeof(errstr),
        "rte_ring_create(%s, %zd, %u)",
        rn.c_str(), count, socket_id);
    throw slankdev::exception(errstr);
  }

  kernel_log("Construct ring %s \n", ring_->name);
}


Ring_dpdk::~Ring_dpdk()
{
  if (ring_) {
    kernel_log("Destruct ring %s \n", ring_->name);
    rte_ring_free(ring_);
  }
}


void Ring_dpdk::push_bulk(rte_mbuf** obj_table, size_t n)
{
  int ret = rte_ring_enqueue_bulk(ring_,reinterpret_cast<void**>(obj_table),n);
  if (ret < 0) {
    if (ret == -EDQUOT ) {
      /*
       * Quota exceeded.
       * The objects have been enqueued,
       * but the high water mark is exceeded.
       */
    }
    else if (ret == -ENOBUFS) {
      /*
       * Not enough room in the ring to enqueue;
       * no object is enqueued.
       */
      struct rte_mbuf* pkts[n];
      bool ret = pop_bulk(pkts, n);
      if (ret) rte_pktmbuf_free_bulk(pkts, n);
      push_bulk(obj_table, n);
    } else {
      throw slankdev::exception("rte_ring_enqueue_bulk: unknown");
    }
  }
}


bool Ring_dpdk::pop_bulk(rte_mbuf** obj_table, size_t n)
{
  int ret = rte_ring_dequeue_bulk(ring_,reinterpret_cast<void**>(obj_table),n);
  if (ret < 0) {
    if (ret == -ENOENT) {
      /*
       * Not enough entries in the ring to dequeue,
       * no object is dequeued.
       */
    }
    return false;
  }
  return true;
}
#endif




Rxq::Rxq(uint16_t pid, uint16_t qid, size_t size)
  : port_id(pid)
  , queue_id(qid)
{
  kernel_log("Construct Rxq %u:%u\n", pid, qid);
  std::string name = "PORT" + std::to_string(pid) + "RX" + std::to_string(qid);

  size_t mbuf_cache_size = 0;
  size_t mbuf_siz = RTE_MBUF_DEFAULT_BUF_SIZE;
  size_t num_mbufs = 8192;
  mempool.create(
      name.c_str(),
      num_mbufs ,
      mbuf_cache_size, mbuf_siz,
      rte_socket_id()
      );

  int socket_id = rte_socket_id();
  int retval = rte_eth_rx_queue_setup(pid, qid, size,
                          socket_id, NULL, mempool.get_raw());
  if (retval < 0)
    throw slankdev::exception("rte_eth_rx_queue_setup failed");
}
size_t Rxq::burst(struct rte_mbuf** rx_pkts, size_t bulk_size)
{
  uint16_t nb_rx = rte_eth_rx_burst(port_id, queue_id, rx_pkts, bulk_size);
  return nb_rx;
}





Txq::Txq(uint16_t pid, uint16_t qid, size_t size)
  : port_id(pid)
  , queue_id(qid)
{
  kernel_log("Construct Txq %u:%u\n", pid, qid);
  int socket_id = rte_socket_id();
  int retval = rte_eth_tx_queue_setup(pid, qid, size, socket_id, NULL);
  if (retval < 0)
    throw slankdev::exception("rte_eth_rx_queue_setup failed");
}
void Txq::burst(struct rte_mbuf** pkts, size_t bulk_size)
{
  uint16_t nb_tx = rte_eth_tx_burst(port_id, queue_id, pkts, bulk_size);
  if (nb_tx != bulk_size) {
    rte_pktmbuf_free_bulk(&pkts[nb_tx], bulk_size-nb_tx);
  }
}



