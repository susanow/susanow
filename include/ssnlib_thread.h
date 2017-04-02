

/*-
 * MIT License
 *
 * Copyright (c) 2017 Susanow
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
 * @file ssnlib_thread.h
 * @brief definition abstract thread class
 * @author slankdev
 */


#pragma once

#include <string>
#include <slankdev/extra/dpdk_header.h>
#include <slankdev/exception.h>
#include <ssnlib_log.h>
#include <slankdev/vty.h>
#include <lthread_api.h>




struct Fthread {
  const std::string name;
  Fthread(const char* n);
  virtual ~Fthread();
  virtual void impl() = 0;
  virtual void kill() = 0;
  static int spawner(void* arg);
};

class Lthread {
 protected:
  bool running;
 public:
  const std::string name;
  Lthread(const char* n);
  virtual ~Lthread();
  virtual void impl() = 0;
  virtual void kill() = 0;
  bool is_run() const;
  static void spawner(void* arg);
};

struct Tthread {
  const std::string name;
  Tthread(const char* n);
  virtual ~Tthread();
  virtual void impl() = 0;
  static void spawner(struct rte_timer *, void *arg);
};



template <class T>
struct Thread_pool_TMP {
  std::vector<T*> threads;
 public:
  virtual ~Thread_pool_TMP() { for (T* t : threads) delete t; }
  void add_thread(T* t) { threads.push_back(t); }
  size_t size() const { return threads.size(); }
  const T* get_thread(size_t i) const { return threads[i]; }
  T* get_thread(size_t i) { return threads[i]; }
  T* find_name2ptr(const std::string& name)
  {
    for (T* t : threads) {
      if (t->name == name) return t;
    }
    return nullptr;
  }
};


using Fthread_pool = Thread_pool_TMP<Fthread>;
using Lthread_pool = Thread_pool_TMP<Lthread>;
using Tthread_pool = Thread_pool_TMP<Tthread>;


class vty_thread : public Fthread {
  slankdev::vty vty_;
 public:
  vty_thread(void* userptr);
  void install_command(slankdev::command* cmd) { vty_.install_command(cmd); }
  virtual void impl() override { vty_.dispatch(); }
  virtual void kill() override {}
};


class lthread_sched : public Fthread {
  Lthread_pool& slowthreads;
  std::vector<struct lthread*> lts;
 public:
  lthread_sched(Lthread_pool& p) : Fthread("lthread_sched"), slowthreads(p) {}
  virtual void impl() override;
  virtual void kill() override {}
  void launch_lthread(Lthread* lthread);
  void kill_lthread(Lthread* lthread);
  void show(slankdev::shell* sh) const;
};




