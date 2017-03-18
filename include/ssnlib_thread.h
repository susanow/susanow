
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
 * @file ssnlib_thread.h
 * @brief definition abstract thread class
 * @author slankdev
 */


#pragma once

#include <string>
#include <slankdev/exception.h>
#include <ssnlib_log.h>
#include <slankdev/vty.h>
#include <lthread_api.h>

namespace ssnlib {

struct Fthread {
    const std::string name;
    Fthread(const char* n) : name(n)
    { kernel_log("Construct thread %s\n", name.c_str()); }
    virtual ~Fthread() { kernel_log("Destruct thread %s \n", name.c_str()); }
    virtual void impl() = 0;
};

struct Lthread {
    const std::string name;
    Lthread(const char* n) : name(n)
    { kernel_log("Construct lthread %s\n", name.c_str()); }
    virtual ~Lthread() { kernel_log("Destruct lthread %s \n", name.c_str()); }
    virtual void impl() = 0;
};

struct Tthread {
    const std::string name;
    Tthread(const char* n) : name(n)
    { kernel_log("Construct tthread %s\n", name.c_str()); }
    virtual ~Tthread() { kernel_log("Destruct tthread %s \n", name.c_str()); }
    virtual void impl() = 0;
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



const char* str = "\r\n"
    "Hello, this is Susanow (version 0.00.00.0).\r\n"
    "Copyright 2017-2020 Hiroki SHIROKURA.\r\n"
    "\r\n"
    " .d8888b.                                                             \r\n"
    "d88P  Y88b                                                            \r\n"
    "Y88b.                                                                 \r\n"
    " \"Y888b.   888  888 .d8888b   8888b.  88888b.   .d88b.  888  888  888 \r\n"
    "    \"Y88b. 888  888 88K          \"88b 888 \"88b d88\"\"88b 888  888  888 \r\n"
    "      \"888 888  888 \"Y8888b. .d888888 888  888 888  888 888  888  888 \r\n"
    "Y88b  d88P Y88b 888      X88 888  888 888  888 Y88..88P Y88b 888 d88P \r\n"
    " \"Y8888P\"   \"Y88888  88888P\' \"Y888888 888  888  \"Y88P\"   \"Y8888888P\"  \r\n"
    "\r\n";

class vty_thread : public ssnlib::Fthread {
    slankdev::vty vty_;
public:
    vty_thread(void* userptr) : Fthread("vty_thread"), vty_(9999, str, "Susanow> ")
    { vty_.user_ptr = userptr; }

    void install_command(slankdev::command* cmd)
    { vty_.install_command(cmd); }

    void impl()
    { vty_.dispatch(); }
};



class lthread_sched : public ssnlib::Fthread {
    static void lthread_start(void* arg)
    {
        Lthread* thread = reinterpret_cast<Lthread*>(arg);
        while (1) {
            thread->impl();
            lthread_yield ();
        }
        lthread_exit (NULL);
    }
    Lthread_pool& slowthreads;
public:
    lthread_sched(Lthread_pool& p) : Fthread("lthread_sched"), slowthreads(p) {}
    void impl()
    {
        size_t nb_threads = slowthreads.size();
        printf("Lthread: Launch %zd threads...\n", nb_threads);
        struct lthread *lt[nb_threads];
        for (size_t i=0; i<nb_threads; i++) {
            lthread_create (
                    &lt[i], -1,
                    lthread_sched::lthread_start,
                    slowthreads.get_thread(i)
            );
        }
        lthread_run();
        printf("Lthread finished \n");
    }
    bool kill()
    {
        force_quit = true;
        return true;
    }
};


} /* namespace ssnlib */
