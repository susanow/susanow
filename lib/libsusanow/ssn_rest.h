
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
#include <map>
#include <vector>
#include <poll.h>

typedef void (*ssn_restcb_t)(int,const void*,size_t,void*);

class Rest_server {
  using pr = std::pair<ssn_restcb_t,void*>;
  std::map<std::string,pr> cbs;
  std::vector<struct pollfd> fds;
 public:
  Rest_server(uint32_t addr, uint16_t port);
  void add_route(const char* path, ssn_restcb_t callback, void* arg);
  void dispatch();
};

class ssn_rest {
  Rest_server serv;
 public:
  ssn_rest(uint32_t addr, uint16_t port);
  void add_route(const char* path, ssn_restcb_t cb, void* arg);
  void dispatch();
};

void ssn_rest_poll_thread(void* arg);
void ssn_rest_poll_thread_stop();




