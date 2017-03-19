
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
#include <string>
#include <exception>
#include <sstream>
#include <ostream>

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




class Mempool {
    rte_mempool* raw_;
public:
    Mempool() : raw_(nullptr) {}
    Mempool(const char* name, size_t nb_seg, size_t cache_siz,
            size_t mbuf_siz, uint16_t sock_id) : Mempool()
    {
        create(name, nb_seg, cache_siz,
            mbuf_siz, sock_id);
    }
    ~Mempool()
    {
        free();
    }
    void create(const char* name,
            size_t nb_seg, size_t cache_siz,
            size_t mbuf_siz, uint16_t sock_id)
    {
        if (raw_)
            throw slankdev::exception("already created");

        raw_ = rte_pktmbuf_pool_create(name,
                        nb_seg, cache_siz, 0,
                        mbuf_siz, sock_id);
        if (!raw_) {
            fprintf(stderr, "name     : %s  \n", name     );
            fprintf(stderr, "nb_seg   : %zd \n", nb_seg   );
            fprintf(stderr, "cache_siz: %zd \n", cache_siz);
            fprintf(stderr, "mbuf_siz : %zd \n", mbuf_siz );
            fprintf(stderr, "sock_id  : %u  \n", sock_id  );

            throw slankdev::exception("can not create pool");
        }
    }
    void free()
    {
        if (raw_) {
            rte_mempool_free(raw_);
            raw_ = nullptr;
        }
    }
    rte_mempool* get_raw() { return raw_; }
};


