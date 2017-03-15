
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

class Thread {
public:
    const std::string name;
    Thread(const char* n) : name(n)
    { kernel_log("Construct thread %s\n", name.c_str()); }
    virtual ~Thread() { kernel_log("Destruct thread %s \n", name.c_str()); }
    virtual void impl() { printf("not set thread \n"); }
    virtual bool kill()
    {
        throw slankdev::exception("kill() is not implemented yet.");
    }
};




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
class vty_thread : public ssnlib::Thread {
    slankdev::vty vty_;
    struct quit : public slankdev::vty::cmd_node {
        quit() : cmd_node("quit") {}
        void function(slankdev::vty::shell* sh) { sh->close(); }
    };
    struct clear : public slankdev::vty::cmd_node {
        clear() : cmd_node("clear") {}
        void function(slankdev::vty::shell* sh)
        {
            sh->Printf("\033[2J\r\n");
        }
    };
public:
    vty_thread(void* userptr) : Thread("vty_thread"), vty_(9999, str)
    {
        vty_.user_ptr = userptr;
        install_command(new quit );
        install_command(new clear);
    }
    void install_command(slankdev::vty::cmd_node* cmd)
    {
        vty_.add_command(cmd);
    }
    void impl()
    {
        vty_.dispatch();
    }
    bool kill() { vty_.halt(); return true; }
};


struct Lthread {
    const std::string name;
    Lthread(const char* n) : name(n) {}
    virtual void impl() = 0;
};



class lthread_sched : public ssnlib::Thread {
    static void lthread_start(void* arg)
    {
        Lthread* thread = reinterpret_cast<Lthread*>(arg);
        while (1) {
            thread->impl();
            lthread_yield ();
        }
        lthread_exit (NULL);
    }
    std::vector<Lthread*> slowthreads;
public:
    lthread_sched() : Thread("lthread_sched") {}
    void impl()
    {
        printf("%zd threads\n", slowthreads.size());
        struct lthread *lt[slowthreads.size()];
        lthread_create (&lt[0], -1, lthread_sched::lthread_start, slowthreads[0]);
        lthread_create (&lt[1], -1, lthread_sched::lthread_start, slowthreads[1]);
        lthread_run();
        printf("lthread finished \n");
    }
    size_t size() const { return slowthreads.size(); }
    void add_thread(Lthread* th) { slowthreads.push_back(th); }
    const Lthread* get_thread(size_t i) const { return slowthreads[i]; }
    bool kill()
    {
        force_quit = true;
        return true;
    }
};


} /* namespace ssnlib */
