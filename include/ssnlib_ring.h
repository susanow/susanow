


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



#pragma once

#include <queue> // for Ring_stdqueue
#include <mutex> // lock

#include <ssnlib_mempool.h>





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
    void push_bulk(rte_mbuf** obj_table, size_t n) override;
    bool pop_bulk(rte_mbuf** obj_table, size_t n) override;
    size_t count() const override { auto_lock lg(*m); return queue_.size(); }
    size_t size()  const override { auto_lock lg(*m); return queue_.size(); }
    bool   empty() const override { auto_lock lg(*m); return queue_.empty(); }
};



class Ring_dpdk : public Ring_interface {
protected:
    struct rte_ring* ring_;
	size_t ring_depth;
public:
    Ring_dpdk(size_t count, size_t p, size_t q, const char* nameprefix);
    virtual ~Ring_dpdk();
    Ring_dpdk(const Ring_dpdk&) = default;
    Ring_dpdk(Ring_dpdk&&) = default;
    void push_bulk(rte_mbuf** obj_table, size_t n) override;
    bool pop_bulk(rte_mbuf** obj_table, size_t n) override;
	virtual size_t count() const override { return rte_ring_count(ring_);    }
    virtual size_t size()  const override { return ring_depth;               }
    virtual bool   empty() const override { return rte_ring_empty(ring_)==1; }
};




class Rxq_interface {
    Ring_dpdk ring_impl;
    Mempool           mempool;
    const uint16_t port_id;
    const uint16_t queue_id;
public:
    Rxq_interface(uint16_t pid, uint16_t qid, size_t size);
    Rxq_interface(const Rxq_interface&) = default;
    Rxq_interface(Rxq_interface&&) = default;
    virtual void burst_bulk();
    void push_bulk(rte_mbuf** obj_table, size_t n) { ring_impl.push_bulk(obj_table, n); }
    bool pop_bulk(rte_mbuf** obj_table, size_t n)  { return ring_impl.pop_bulk(obj_table, n); }
    size_t count() const  { return ring_impl.count(); }
    size_t size() const   { return ring_impl.size();  }
    bool empty() const    { return ring_impl.empty(); }
};




class Txq_interface {
    Ring_dpdk ring_impl;
    const uint16_t port_id;
    const uint16_t queue_id;
public:
    Txq_interface(uint16_t pid, uint16_t qid, size_t size);
    Txq_interface(const Txq_interface&) = default;
    Txq_interface(Txq_interface&&) = default;

    virtual void burst_bulk();
    void push_bulk(rte_mbuf** obj_table, size_t n)  { ring_impl.push_bulk(obj_table, n); }
    bool pop_bulk(rte_mbuf** obj_table, size_t n)  { return ring_impl.pop_bulk(obj_table, n); }
    size_t count() const  { return ring_impl.count(); }
    size_t size() const   { return ring_impl.size();  }
    bool empty() const    { return ring_impl.empty(); }
};



