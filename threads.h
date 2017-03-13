
#pragma once

#include <slankdev/vty.h>
#include <ssnlib_thread.h>


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
public:
    vty_thread() : Thread("vty_thread"), vty_(9999, str)
    {
        vty_.add_command(new clear);
        vty_.add_command(new halt);
        vty_.add_command(new show);
        vty_.add_command(new quit);
    }
    void impl()
    {
        vty_.dispatch();
    }
    bool kill() { vty_.halt(); return true; }
};



struct slow_thread {
    int a;
    slow_thread(int b) : a(b) {}
    void impl()
    {
        printf("impl %d\n", a);
        sleep(1);
    }
};
#include <lthread_api.h>
class lthread_sched : public ssnlib::Thread {
    static void lthread_start(void* arg)
    {
        slow_thread* thread = reinterpret_cast<slow_thread*>(arg);
        while (1) {
            thread->impl();
            lthread_yield ();
        }
        lthread_exit (NULL);
    }
public:
    lthread_sched() : Thread("lthread_sched") {}
    void impl()
    {
        struct lthread *lt[2];
        lthread_create (&lt[0], -1, lthread_sched::lthread_start, new slow_thread(0));
        lthread_create (&lt[1], -1, lthread_sched::lthread_start, new slow_thread(1));
        lthread_run ();
        printf("lthread finished \n");
    }
    bool kill()
    {
        force_quit = true;
        return true;
    }
};


