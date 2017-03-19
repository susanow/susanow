
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
 * @file ssnlib_mempool.h
 * @brief wrap rte_mempool.h
 * @author slankdev
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <rte_mbuf.h>



class Mempool {
    rte_mempool* raw_;
public:
    Mempool() : raw_(nullptr) {}
    Mempool(const char* name, size_t nb_seg, size_t cache_siz,
            size_t mbuf_siz, uint16_t sock_id) : Mempool()
    { create(name, nb_seg, cache_siz, mbuf_siz, sock_id); }
    ~Mempool() { free(); }
    void create(const char* name, size_t nb_seg,
            size_t cache_siz, size_t mbuf_siz, uint16_t sock_id);
    void free();
    rte_mempool* get_raw() { return raw_; }
};




