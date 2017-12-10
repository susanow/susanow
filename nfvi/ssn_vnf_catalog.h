
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


/**
 * @brief vnf catalog class
 * @details
 *   This class provides vnf-allocationing to dynamic vnf using.
 */
class ssn_vnf_catalog final {

  struct catalog_ele {
    std::string name;
    ssn_vnfallocfunc_t allocator;
    catalog_ele(std::string n, ssn_vnfallocfunc_t f) : name(n), allocator(f) {}
  }; /* struct catalog_ele */

  std::vector<catalog_ele> catalog;

 public:

  /**
   * @brief get catalog elements size
   * @return number of elements
   */
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
   * @brief Allocate new VNF
   * @param [in] cname catalog-name
   * @param [in] iname instance-name
   * @return valid-pointer success allocation
   * @return nullptr unsuccess, maybe invalid cname or iname
   */
  ssn_vnf* alloc_vnf(const char* cname, const char* iname)
  {
    size_t n_cat = catalog.size();
    for (size_t i=0; i<n_cat; i++) {
      if (catalog[i].name == cname)
        return catalog[i].allocator(iname);
    }
    return nullptr;
  }

  /**
   * @brief Register new VNF-catalog
   * @param [in] cname catalog-name
   * @param [in] allocator allocator function
   * @return 0 success
   * @return -1 cname is already registered
   */
  int register_vnf(const char* cname, ssn_vnfallocfunc_t allocator)
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
   * @brief Unregister VNF catalog
   * @param cname catalog-name
   * @return 0 success
   * @return -1 cname is invalid
   */
  int unregister_vnf(const char* cname)
  {
    const size_t n_ele = catalog.size();
    for (size_t i=0; i<n_ele; i++) {
      if (catalog[i].name == cname) {
        catalog.erase(catalog.begin() + i);
        return 0;
      }
    }
    return -1;
  }

}; /* class ssn_vnf_catalog */


