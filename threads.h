
#pragma once

#include <ssnlib_thread.h>
#include <slankdev/string.h>


struct slow_thread_test : public ssnlib::Lthread {
    int a;
    slow_thread_test(int b) : Lthread(
            slankdev::fs("slowthread(%d)", b).c_str()
            ), a(b) {}
    void impl()
    {
        printf("impl %d\n", a);
        sleep(1);
    }
};




