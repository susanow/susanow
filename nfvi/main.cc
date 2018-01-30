
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

#include <pch.h>

#include <ssn_nfvi.h>
#include <ssn_rest_api.h>
#include <vnfs/l2fwd.h>
#include <vnfs/l2fwd_delay.h>
#include <vnfs/l2fwd_numa.h>
#include <vnfs/l3fwd.h>
#include <vnfs/acl.h>



int main(int argc, char** argv)
{
  ssn_nfvi nfvi(argc, argv);
  ssn_vnf_l2fwd_delay_block::n_delay = 100;
  nfvi.vnf_register_to_catalog("l2fwd"      , ssn_vnfalloc_l2fwd);
  nfvi.vnf_register_to_catalog("l2fwd_delay", ssn_vnfalloc_l2fwd_delay);
  nfvi.vnf_register_to_catalog("l2fwd_numa" , ssn_vnfalloc_l2fwd_numa);
  nfvi.vnf_register_to_catalog("l3fwd"      , ssn_vnfalloc_l3fwd);
  nfvi.vnf_register_to_catalog("acl"        , ssn_vnfalloc_acl);

  nfvi.run(8888);
  printf("bye...\n");
}



