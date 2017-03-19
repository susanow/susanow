

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
 * @file ssnlib_port.h
 * @brief port management library
 * @author slankdev
 */


#pragma once
#include <unistd.h>

#include <ssnlib_mempool.h>
#include <ssnlib_ring.h>
#include <ssnlib_log.h>
#include <ssnlib_port_impl.h>



using Rxq    = Rxq_interface<Ring_dpdk>;
using Txq    = Txq_interface<Ring_dpdk>;
class Port {
public:
    size_t nb_rx_rings;
    size_t nb_tx_rings;
    size_t rx_ring_size;
    size_t tx_ring_size;
    Ether_addr        addr;
    port_conf         conf;
    port_stats        stats;
    link_stats        link;
    dev_info          info;
    const uint8_t     id;
    const std::string name;

    std::vector<Rxq> rxq;
    std::vector<Txq> txq;

public:
    Port(size_t port_id);
    ~Port();
    void init();
    void fini();
    void linkup();
    void linkdown() { rte_eth_dev_set_link_down(id); }
    void devstart();
    void devstop () { rte_eth_dev_stop (id); }
    void promisc_enable()  { rte_eth_promiscuous_enable(id);  }
    void promisc_disable() { rte_eth_promiscuous_disable(id); }
    bool is_promiscuous() { return rte_eth_promiscuous_get(id)==1; }
    void configure();
};





