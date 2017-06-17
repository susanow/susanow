




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
 * @file thread/test.h
 * @author slankdev
 */

#pragma once

#include <ssnlib_sys.h>
#include <ssnlib_thread.h>
#include <slankdev/string.h>


class lthread_test : public Lthread {
 public:
  int a;
  lthread_test(int b)
    : Lthread(slankdev::fs("lthread_test(%d)", b).c_str()),
    a(b) {}
  virtual void impl() override {
    printf("lthread_test(%d)\n", a);
    lthread_sleep(1 * 1000 * 1000 * 1000);
  }
  virtual void kill() override {
    running = false;
  }
};



class fthread_test : public Fthread {
  int a;
  bool running;
 public:
  fthread_test(int b)
    : Fthread(slankdev::fs("fthread_test(%d)", b).c_str()),
    a(b),
    running(false) {}
  virtual void impl() override {
    running = true;
    while (running) {
      printf("fthread_test(%d)\n", a);;
      sleep(1);
    }
  }
  virtual void kill() override { running = false; }
};

