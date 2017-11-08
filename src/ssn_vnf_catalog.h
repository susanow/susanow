
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

#include <ssn_types.h>
#include <ssn_vnf_v02.h>
#include <ssn_vnf_v02_l2fwd1b.h>
#include <ssn_vnf_v02_l2fwd2b.h>


/**
 * @brief vnf catalog class
 * @details
 *   This class provides vnf-allocation to dynamic vnf using.
 */
class ssn_vnf_catalog final {

  struct catalog_ele {
    std::string name;
    ssn_vnfallocfunc_t allocator;
    catalog_ele(std::string n, ssn_vnfallocfunc_t f) : name(n), allocator(f) {}
  }; /* struct catalog_ele */

  std::vector<catalog_ele> catalog;

 public:

  ssn_vnf* alloc_vnf(const char* catalog_name, const char* instance_name)
  {
    size_t n_cat = catalog.size();
    for (size_t i=0; i<n_cat; i++) {
      if (catalog[i].name == catalog_name)
        return catalog[i].allocator(instance_name);
    }
    throw slankdev::exception("ssn_vnf_catalog::alloc_vnf: not found vnf");
  }

  void register_vnf(const char* catalog_name, ssn_vnfallocfunc_t allocator)
  { catalog.emplace_back(catalog_name, allocator); }

  void unregister_vnf(const char* catalog_name)
  {
    size_t n_cat = catalog.size();
    for (size_t i=0; i<n_cat; i++) {
      if (catalog[i].name == catalog_name) {
        catalog.erase(catalog.begin() + i);
        return ;
      }
    }
    throw slankdev::exception("ssn_vnf_catalog::register_vnf: not found vnf");
  }

}; /* class ssn_vnf_catalog */


