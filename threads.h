
#pragma once

#include <ssnlib_thread.h>
#include <slankdev/string.h>


struct slow_thread_test : public ssnlib::slow_thread {
    int a;
    slow_thread_test(int b) : slow_thread(
            slankdev::fs("slowthread(%d)", b).c_str()
            ), a(b) {}
    void impl()
    {
        printf("impl %d\n", a);
        sleep(1);
    }
};




