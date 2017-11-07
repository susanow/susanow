
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

#pragma once
#include <stdint.h>
#include <stddef.h>

void ssn_port_stat_debug_dump(FILE* fp, size_t port_id);
void ssn_port_stat_update(void*);
void ssn_port_stat_init();
void ssn_port_stat_fin();

size_t ssn_port_stat_get_cur_rx_pps(size_t pid);
size_t ssn_port_stat_get_cur_tx_pps(size_t pid);
size_t ssn_port_stat_get_cur_rx_bps(size_t pid);
size_t ssn_port_stat_get_cur_tx_bps(size_t pid);
size_t ssn_port_stat_get_cur_rx_mis(size_t pid);

size_t ssn_port_stat_get_tot_rx_packets(size_t pid);
size_t ssn_port_stat_get_tot_tx_packets(size_t pid);
size_t ssn_port_stat_get_tot_rx_bytes(size_t pid);
size_t ssn_port_stat_get_tot_tx_bytes(size_t pid);


