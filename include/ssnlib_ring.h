


/*-
 * MIT License
 *
 * Copyright (c) 2017 Susanoo G
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



#pragma once
#include <ssnlib_log.h>
#include <ssnlib_mempool.h>
#include <queue> // for Ring_stdqueue
#include <mutex> // lock


static inline void __attribute__((always_inline))
rte_pktmbuf_free_bulk(struct rte_mbuf *m_list[], int16_t npkts)
{
	while (npkts--) {
		rte_pktmbuf_free(*m_list++);
    }
}


class Ring_interface {
public:
    virtual void push_bulk(rte_mbuf** obj_table, size_t n) = 0;
    virtual bool pop_bulk(rte_mbuf** obj_table, size_t n)  = 0;
    virtual size_t count() const = 0;
    virtual size_t size() const  = 0;
    virtual bool empty() const   = 0;
};


class Ring_stdqueue : public Ring_interface {
    std::queue<rte_mbuf*> queue_;
    mutable std::mutex* m;
    using auto_lock = std::lock_guard<std::mutex>;
public:
    Ring_stdqueue(size_t, size_t, size_t, const char*) { m = new std::mutex; }
    virtual ~Ring_stdqueue() { delete(m); }
    Ring_stdqueue(const Ring_stdqueue&) { m = new std::mutex; }
    Ring_stdqueue(Ring_stdqueue&&) = default;
    void push_bulk(rte_mbuf** obj_table, size_t n) override
    {
        auto_lock lg(*m);
        for (size_t i=0; i<n; i++) {
            queue_.push(obj_table[i]);
        }
    }
    bool pop_bulk(rte_mbuf** obj_table, size_t n) override
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
    size_t count() const override { auto_lock lg(*m); return queue_.size(); }
    size_t size()  const override { auto_lock lg(*m); return queue_.size(); }
    bool   empty() const override { auto_lock lg(*m); return queue_.empty(); }
};

class Ring_dpdk : public Ring_interface {
protected:
    struct rte_ring* ring_;
	size_t ring_depth;
public:
    Ring_dpdk(size_t count, size_t p, size_t q, const char* nameprefix) : ring_depth(count)
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
    virtual ~Ring_dpdk()
    {
        if (ring_) {
            kernel_log("Destruct ring %s \n", ring_->name);
            rte_ring_free(ring_);
        }
    }
    Ring_dpdk(const Ring_dpdk&) = default;
    Ring_dpdk(Ring_dpdk&&) = default;

    void push_bulk(rte_mbuf** obj_table, size_t n) override
    {
        int ret = rte_ring_enqueue_bulk(ring_, reinterpret_cast<void**>(obj_table), n);
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
    bool pop_bulk(rte_mbuf** obj_table, size_t n) override
    {
        int ret = rte_ring_dequeue_bulk(ring_, reinterpret_cast<void**>(obj_table), n);
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
	virtual size_t count() const override { return rte_ring_count(ring_);    }
    virtual size_t size()  const override { return ring_depth;               }
    virtual bool   empty() const override { return rte_ring_empty(ring_)==1; }
};



template <class RING_INTERFACE>
class Rxq_interface {
    RING_INTERFACE ring_impl;
    Mempool           mempool;
    const uint16_t port_id;
    const uint16_t queue_id;
public:
    Rxq_interface(uint16_t pid, uint16_t qid, size_t size)
        : ring_impl(size, pid, qid, "RX"),
        port_id(pid),
        queue_id(qid)
    {

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
        int retval = rte_eth_rx_queue_setup(pid, qid, size, socket_id, NULL, mempool.get_raw());
        if (retval < 0)
            throw slankdev::exception("rte_eth_rx_queue_setup failed");
    }
    Rxq_interface(const Rxq_interface&) = default;
    Rxq_interface(Rxq_interface&&) = default;
    virtual void burst_bulk()
    {
        size_t bulk_size = 32;
        struct rte_mbuf* rx_pkts[bulk_size];
        uint16_t nb_rx = rte_eth_rx_burst(port_id, queue_id, rx_pkts, bulk_size);
        if (nb_rx == 0) return;
        push_bulk(rx_pkts, nb_rx);
    }
    void push_bulk(rte_mbuf** obj_table, size_t n)  { ring_impl.push_bulk(obj_table, n); }
    bool pop_bulk(rte_mbuf** obj_table, size_t n)  { return ring_impl.pop_bulk(obj_table, n); }
    size_t count() const  { return ring_impl.count(); }
    size_t size() const   { return ring_impl.size();  }
    bool empty() const    { return ring_impl.empty(); }
};


template <class RING_INTERFACE>
class Txq_interface {
    RING_INTERFACE ring_impl;
    const uint16_t port_id;
    const uint16_t queue_id;
public:
    Txq_interface(uint16_t pid, uint16_t qid, size_t size)
        : ring_impl(size, pid, qid, "TX"),
        port_id(pid),
        queue_id(qid)
    {
        int socket_id = rte_socket_id();
        int retval = rte_eth_tx_queue_setup(pid, qid, size, socket_id, NULL);
        if (retval < 0)
            throw slankdev::exception("rte_eth_rx_queue_setup failed");
    }
    Txq_interface(const Txq_interface&) = default;
    Txq_interface(Txq_interface&&) = default;

    virtual void burst_bulk()
    {
        size_t bulk_size = 32;
        struct rte_mbuf* pkts[bulk_size];
        bool ret = pop_bulk(pkts, bulk_size);
        if (ret == true) {
            uint16_t nb_tx = rte_eth_tx_burst(port_id, queue_id, pkts, bulk_size);
            if (nb_tx != bulk_size) {
                rte_pktmbuf_free_bulk(&pkts[nb_tx], bulk_size-nb_tx);
            }
        }
    }
    void push_bulk(rte_mbuf** obj_table, size_t n)  { ring_impl.push_bulk(obj_table, n); }
    bool pop_bulk(rte_mbuf** obj_table, size_t n)  { return ring_impl.pop_bulk(obj_table, n); }
    size_t count() const  { return ring_impl.count(); }
    size_t size() const   { return ring_impl.size();  }
    bool empty() const    { return ring_impl.empty(); }
};



