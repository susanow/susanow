

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
 * @details
 *   ssn_ma_port module.
 *   Provide abstruction layer to access physical
 *   dpdk port from multi-thread. (ma: multi-accessable)
 */

#pragma once
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

struct rte_mbuf;

void ssn_ma_port_debug_dump(FILE* fp);

void ssn_ma_port_link_up(size_t pid);
void ssn_ma_port_link_down(size_t pid);
void ssn_ma_port_promisc_on(size_t pid);
void ssn_ma_port_promisc_off(size_t pid);
void ssn_ma_port_dev_up(size_t pid);
void ssn_ma_port_dev_down(size_t pid);

void ssn_ma_port_configure_hw(size_t dpdk_port_id, size_t n_rxq, size_t n_txq);
void ssn_ma_port_configure_acc(size_t dpdk_port_id, size_t n_rxacc, size_t n_txacc);

size_t ssn_ma_port_rx_burst(size_t port_id, size_t aid, rte_mbuf** mbufs, size_t n_mbufs);
size_t ssn_ma_port_tx_burst(size_t port_id, size_t aid, rte_mbuf** mbufs, size_t n_mbufs);


