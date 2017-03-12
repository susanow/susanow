

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <slankdev/exception.h>


namespace slankdev {

inline uint64_t rdtsc()
{
    unsigned int eax, edx;
    __asm__ volatile("rdtsc" : "=a"(eax), "=d"(edx));
    return ((uint64_t)edx << 32) | eax;
}

inline void delay_clk(size_t num_clks)
{
    uint64_t begin = rdtsc();
    while (rdtsc() - begin < num_clks) ;
}

inline void daemon()
{
    if (::daemon(0, 0) != 0) {
        throw slankdev::exception("daemon");
    }
}


} /* namespace slankdev */
