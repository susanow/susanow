
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
#include <ssn_vnf.h>
#include <ssn_vnf_port.h>
#include <ssn_vnf_port_dpdk.h>
#include <ssn_vnf_port_virt.h>


/**
 * @brief vnf_port catalog class
 * @details
 *   This class provides port-allocationing to dynamic vnf using
 */
class ssn_port_catalog final {

  struct catalog_ele {
    std::string name;
    ssn_portallocfunc_t allocator;
    catalog_ele(std::string n, ssn_portallocfunc_t f) : name(n), allocator(f) {}
  }; /* struct catalog_ele */

  std::vector<catalog_ele> catalog;

 public:

  size_t size() const { return catalog.size(); }

  /**
   * @brief get element reference for debugging
   * @param [in] num catalog index
   * @return catalog-element
   * @details
   *    if num is invalid, this operation is undefined
   */
  const catalog_ele& operator[](size_t num) const { return catalog[num]; }

  /**
   * @brief get element reference for debugging with range-check
   * @param [in] num catalog index
   * @return catalog-element
   * @details
   *    if num is invalid, this operation throw exception.
   */
  const catalog_ele& at(size_t num) const
  {
    if (num >= size()) throw slankdev::exception("range error");
    return catalog[num];
  }

  /**
   * @brief allocate new port from port-catalog
   * @param [in] cname catalog-name
   * @param [in] iname instance-name
   * @param [in] arg func-argument of allocator-function
   * @return nullptr some-error occured
   * @return valid-pointer allocated-port-instance's pointer
   * @details
   *    If cname is invalid, this function throw exception.
   */
  ssn_vnf_port* alloc_port(const char* cname, const char* iname, void* arg)
  {
    size_t n_cat = catalog.size();
    for (size_t i=0; i<n_cat; i++) {
      if (catalog[i].name == cname)
        return catalog[i].allocator(iname, arg);
    }
    throw slankdev::exception("ssn_port_catalog::alloc_port: not found port");
  }

  /**
   * @brief Register new Port-catalog
   * @param [in] cname catalog-name
   * @param [in] allocator allocator function
   * @return 0 success
   * @return -1 cname is already registered
   */
  int register_port(const char* cname, ssn_portallocfunc_t allocator)
  {
    /* Check if cname is already registered */
    const size_t n_ele = catalog.size();
    for (size_t i=0; i<n_ele; i++) {
      if (catalog[i].name == cname)
        return -1;
    }
    catalog.emplace_back(cname, allocator);
    return 0;
  }

  /**
   * @brief Unregister Port catalog
   * @param cname catalog-name
   * @return 0 success
   * @return -1 cname is invalid
   */
  int unregister_port(const char* cname)
  {
    size_t n_cat = catalog.size();
    for (size_t i=0; i<n_cat; i++) {
      if (catalog[i].name == cname) {
        catalog.erase(catalog.begin() + i);
        return 0;
      }
    }
    return -1;
  }

}; /* class ssn_port_catalog */


