

/*-
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
 * @file ssn_config.h
 * @brief wrap rte_cfgfile.h
 * @author Hiroki SHIROKURA
 */

#pragma once
#include <vector>
#include <string>
#include <rte_cfgfile.h>

struct ssn_config_entry {
  std::string name;
  std::string value;
};

struct ssn_config_section {
  std::string name;
  std::vector<ssn_config_entry> entries;
};

class ssn_config {
  rte_cfgfile* cfg;
 public:
  std::vector<ssn_config_section> sections;

  ssn_config() : cfg(nullptr) {}
  ~ssn_config() { close(); }
  void load(const char* filename);
  bool has_section(const char* sec_name);
  bool has_entry(const char* sec_name, const char* ent_name);
  std::string get_entry(const char* sec_name, const char* ent_name);
  void close();
};



