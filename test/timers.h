
#pragma once

#include <ssnlib_timer.h>
#include <ssnlib_sys.h>

namespace ssnlib {

class TimerDefault : public Timer {
    System* sys;
public:
    TimerDefault(System* s) : sys(s) {}
    ~TimerDefault() {}
    void func() override
    {
        sys->cyclic_task();
    }
};


} /* namespace ssnlib */
