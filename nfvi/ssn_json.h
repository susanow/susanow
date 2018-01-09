
/*
 * MIT License
 *
 * Copyright (c) 2017 Susanow
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
#include <crow.h>
class ssn_vnf;
class ssn_vnf_port;
class ssn_vnf_block;
class ssn_vnf_port_patch_panel;

crow::json::wvalue responce_info(bool success, const char* msg);

crow::json::wvalue nfvi_info(const ssn_nfvi* nfvi);
crow::json::wvalue vnf_port_info(const ssn_vnf_port* port);
crow::json::wvalue vnf_block_info(const ssn_vnf_block* block);
crow::json::wvalue vnf_info(const ssn_vnf* vnf);;
crow::json::wvalue ppp_info(const ssn_vnf_port_patch_panel* ppp);

crow::json::wvalue cpu_info(const ssn_nfvi* nfvi, size_t socket_id);
crow::json::wvalue core_info(const ssn_nfvi* nfvi, size_t lcore_id);
crow::json::wvalue mempool_info(const rte_mempool* mp);

