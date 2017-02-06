
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
 * @file ssnlib_timer.h
 * @brief timer libraries
 * @author slankdev
 */

#pragma once
#include <rte_timer.h>
#include <slankdev/exception.h>


namespace ssnlib {



struct Timer : public rte_timer {
    Timer() {}
    virtual ~Timer() {}
    virtual void func() = 0;
    static void doo(struct rte_timer *, void *a)
    {
        Timer* tt = reinterpret_cast<Timer*>(a);
        tt->func();
    }
};


class Timersubsys : public ssnlib::ssn_thread {
    size_t lcore_id;
    std::vector<Timer*> timers;
public:
    Timersubsys(size_t id) : lcore_id(id) {}
    ~Timersubsys()
    {
        while (timers.size() > 0) {
            Timer* tmp = timers.back();
            timers.pop_back();
            delete tmp;
        }
    }
    void init() { rte_timer_subsystem_init(); }
    void add_timer(Timer* newtimer, uint64_t interval_hz)
    {
        rte_timer_init(newtimer);
        rte_timer_reset(newtimer, interval_hz, PERIODICAL, lcore_id, newtimer->doo, newtimer);
        timers.push_back(newtimer);
    }
    void operator()()
    {
        while (true) {
            rte_timer_manage();
        }
    }
};



} /* namespace ssnlib */


