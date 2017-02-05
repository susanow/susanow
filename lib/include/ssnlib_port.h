

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

#include <ssnlib_dpdk.h>
#include <ssnlib_mempool.h>
#include <ssnlib_ring.h>
#include <ssnlib_log.h>
#include <ssnlib_port_impl.h>


namespace ssnlib {


template <class RXQ, class TXQ>
class Port_interface {
public:
    static size_t nb_rx_rings;
    static size_t nb_tx_rings;
    static size_t rx_ring_size;
    static size_t tx_ring_size;

public:
    const uint8_t     id;
    const std::string name;
    ether_addr        addr;

    port_conf         conf;
    port_stats        stats;
    link_stats        link;
    dev_info          info;

    std::vector<RXQ> rxq;
    std::vector<TXQ> txq;

public:
    Port_interface(size_t port_id) :
        id       (port_id),
        name     ("port" + std::to_string(id)),
        addr     (id),
        conf     (id),
        stats    (id),
        link     (id),
        info     (id)
    {
        if (id >= rte_eth_dev_count()) {
            throw slankdev::exception("invalid port id");
        }


        kernel_log(SYSTEM, "boot port%u ... \n", id);
        rte_eth_macaddr_get(id, &addr);
        info.get();

        kernel_log(SYSTEM, "%s address=%s \n", name.c_str(), addr.toString().c_str());

        if (id >= rte_eth_dev_count())
            throw slankdev::exception("port is not exist");
    }
    virtual void boot()
    {
        configure();
        start();
        promiscuous_set(true);
        kernel_log(SYSTEM, "%s configure ... done\n", name.c_str());
    }
    virtual void linkup  ()
    {
        int ret = rte_eth_dev_set_link_up  (id);
        if (ret < 0) {
            throw slankdev::exception("rte_eth_dev_link_up: failed");
        }
    }
    virtual void linkdown() { rte_eth_dev_set_link_down(id); }
    virtual void start()
    {
        int ret = rte_eth_dev_start(id);
        if (ret < 0) {
            throw slankdev::exception("rte_eth_dev_start: failed");
        }
    }
    virtual void stop () { rte_eth_dev_stop (id); }
    virtual void promiscuous_set(bool on)
    {
        if (on) rte_eth_promiscuous_enable(id);
        else    rte_eth_promiscuous_disable(id);
    }
    virtual bool is_promiscuous() { return rte_eth_promiscuous_get(id)==1; }
    virtual void configure()
    {

        conf.raw.rxmode.mq_mode = ETH_MQ_RX_RSS;
        conf.raw.rx_adv_conf.rss_conf.rss_key = nullptr;
        conf.raw.rx_adv_conf.rss_conf.rss_hf  = ETH_RSS_IP;

        int retval = rte_eth_dev_configure(id, nb_rx_rings, nb_tx_rings, &conf.raw);
        if (retval != 0)
            throw slankdev::exception("rte_eth_dev_configure failed");

        rxq.clear();
        rxq.reserve(nb_tx_rings);
        for (uint16_t qid=0; qid<nb_rx_rings; qid++) {
            rxq.emplace_back(id, qid, rx_ring_size);
        }
        txq.clear();
        txq.reserve(nb_tx_rings);
        for (uint16_t qid=0; qid<nb_tx_rings; qid++) {
            txq.emplace_back(id, qid, tx_ring_size);
        }

        kernel_log(SYSTEM, "%s configure \n", name.c_str());
        kernel_log(SYSTEM, "  nb_rx_rings=%zd size=%zd\n", nb_rx_rings, rx_ring_size);
        kernel_log(SYSTEM, "  nb_tx_rings=%zd size=%zd\n", nb_tx_rings, tx_ring_size);
    }
};
template <class CPU, class PORT> size_t Port_interface<CPU, PORT>::nb_rx_rings    = 1;
template <class CPU, class PORT> size_t Port_interface<CPU, PORT>::nb_tx_rings    = 1;
template <class CPU, class PORT> size_t Port_interface<CPU, PORT>::rx_ring_size   = 128;
template <class CPU, class PORT> size_t Port_interface<CPU, PORT>::tx_ring_size   = 512;




} /* namespace ssnlib */


