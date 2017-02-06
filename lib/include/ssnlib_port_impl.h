

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
 * @file ssnlib_port_impl.h
 * @brief include classes that provide port-infomation
 * @author slankdev
 */


#pragma once
#include <ssnlib_dpdk.h>
#include <slankdev/system.h>



namespace ssnlib {



/*
 * This class has dynamically infomations.
 */
class port_conf {
public:
    const size_t id;
    rte_eth_conf raw;
    port_conf(size_t i) : id(i)
    {
        memset(&raw, 0x00, sizeof(raw));
        raw.rxmode.max_rx_pkt_len = ETHER_MAX_LEN;
    }
};

/*
 * This class has dynamically informations.
 */
class port_stats {
    static const size_t IFG = 12;  /* Inter Frame Gap [Byte] */
    static const size_t PAM =  8;  /* Preamble        [Byte] */
    static const size_t FCS =  4;  /* Frame Check Seq [Byte] */
    static const size_t IFO = IFG+PAM+FCS; /* Pack Overhead  */
public:
    const size_t id;
    size_t rx_pps;
    size_t tx_pps;
    size_t rx_bps;
    size_t tx_bps;
    uint64_t last_update;
    struct rte_eth_stats init;
    struct rte_eth_stats cure_prev;
    struct rte_eth_stats cure;

    port_stats(size_t i)
        : id(i), rx_pps(0), tx_pps(0), rx_bps(0), tx_bps(0),
        last_update(0) { reset(); }
    void reset()
    {
        rte_eth_stats_reset(id);
        rte_eth_stats_get(id, &init);
        cure = init;
        cure_prev = init;
        last_update = slankdev::rdtsc();
    }
    void update()
    {
        rte_eth_stats_get(id, &cure);
        uint64_t now = slankdev::rdtsc();
        double diff_time = double(now-last_update)/rte_get_timer_hz();
        rx_pps = (cure.ipackets - cure_prev.ipackets)/diff_time;
        tx_pps = (cure.opackets - cure_prev.opackets)/diff_time;
        rx_bps = (((cure.ibytes - cure_prev.ibytes) + IFO * rx_pps) << 3)/diff_time;
        tx_bps = (((cure.obytes - cure_prev.obytes) + IFO * tx_pps) << 3)/diff_time;
        cure_prev = cure;
        last_update = now;
    }
};


/*
 * This class has dynamically informations.
 */
class link_stats {
public:
    const size_t id;
    struct rte_eth_link raw;
    link_stats(size_t i) : id(i) {}
    void update() { rte_eth_link_get_nowait(id, &raw); }
};

/*
 * This class has statically infomations.
 */
class dev_info {
public:
    const size_t id;
    struct rte_eth_dev_info raw;
    dev_info(size_t i) : id(i) {}
    void get()
    {
        rte_eth_dev_info_get(id, &raw);
    }
};
class ether_addr : public ::ether_addr {
public:
    const size_t id;
    ether_addr(size_t i) : id(i) {}
    void print(FILE* fd) const { fprintf(fd, "%s", toString().c_str()); }
    std::string toString() const
    {
        char buf[32];
        snprintf(buf, sizeof(buf),
                "%02" PRIx8 ":%02" PRIx8 ":%02" PRIx8
                   ":%02" PRIx8 ":%02" PRIx8 ":%02" PRIx8,
                addr_bytes[0], addr_bytes[1],
                addr_bytes[2], addr_bytes[3],
                addr_bytes[4], addr_bytes[5]);
        return buf;
    }
    void update() { rte_eth_macaddr_get(id, this); }
    void set(::ether_addr* addr)
    {
        int ret = rte_eth_dev_default_mac_addr_set(id, addr);
        if (ret < 0) {
            if (ret == -ENOTSUP) {
                throw slankdev::exception(
                        "rte_eth_dev_default_mac_addr_set: hardware doesn't suppoer");
            } else if (ret == -ENODEV) {
                throw slankdev::exception(
                        "rte_eth_dev_default_mac_addr_set: port invalid");
            } else if (ret == -EINVAL) {
                throw slankdev::exception(
                        "rte_eth_dev_default_mac_addr_set: MAC address is invalid");
            } else {
                throw slankdev::exception(
                        "rte_eth_dev_default_mac_addr_set: unknown error");
            }
        }
        update();
    }
    void add(::ether_addr* addr)
    {
        int ret = rte_eth_dev_mac_addr_add(id, addr, 0);
        if (ret < 0) {
            if (ret == -ENOTSUP) {
                throw slankdev::exception(
                "rte_eth_dev_mac_addr_add: hardware doesn't support this feature.");
            } else if (ret == -ENODEV) {
                throw slankdev::exception(
                    "rte_eth_dev_mac_addr_add: port is invalid.");
            } else if (ret == -ENOSPC) {
                throw slankdev::exception(
                    "rte_eth_dev_mac_addr_add: no more MAC addresses can be added.");
            } else if (ret == -EINVAL) {
                throw slankdev::exception(
                    "rte_eth_dev_mac_addr_add: MAC address is invalid.");
            } else {
                throw slankdev::exception("rte_eth_dev_mac_addr_add: unknown");
            }
        }
        update();
    }
    void del(::ether_addr* addr)
    {
        int ret = rte_eth_dev_mac_addr_remove(id, addr);
        if (ret < 0) {
            if (ret == -ENOTSUP) {
                throw slankdev::exception(
                        "rte_eth_dev_mac_addr_remove: hardware doesn't support.");
            } else if (ret == -ENODEV) {
                throw slankdev::exception(
                        "rte_eth_dev_mac_addr_remove: if port invalid.");
            } else if (ret == -EADDRINUSE) {
                std::string errstr = "rte_eth_dev_mac_addr_remove: ";
                errstr += "attempting to remove the default MAC address";
                throw slankdev::exception(errstr.c_str());
            }
        }
        update();
    }
};




} /* namespace ssnlib */


