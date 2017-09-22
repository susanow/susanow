
/*
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
 * @file   ssn_port.h
 * @brief  port management
 * @author Hiroki SHIROKURA
 * @date   2017.9.20
 */

#pragma once
#include <stdint.h>
#include <stddef.h>
#include <rte_ethdev.h>

struct rte_eth_conf;
struct rte_mbuf;

class ssn_port_conf {
 public:
  size_t nb_rxq;
  size_t nb_txq;
  size_t nb_rxd;
  size_t nb_txd;
  ssn_port_conf();
  rte_eth_conf raw;
  void debug_dump(FILE* fp) const;
};

size_t ssn_dev_count();
void ssn_port_link_up(size_t p);
void ssn_port_link_down(size_t p);
void ssn_port_promisc_on(size_t pid);
void ssn_port_promisc_off(size_t pid);
void ssn_port_dev_up(size_t pid);
void ssn_port_dev_down(size_t pid);
void ssn_port_configure(size_t pid, ssn_port_conf* conf);
void ssn_port_init();
void ssn_port_fin();
void ssn_mbuf_free_bulk(rte_mbuf** m_list, size_t npkts);
size_t ssn_port_rx_burst(size_t pid, size_t qid, rte_mbuf** mbufs, size_t nb_mbufs);
size_t ssn_port_tx_burst(size_t pid, size_t qid, rte_mbuf** mbufs, size_t nb_mbufs);

