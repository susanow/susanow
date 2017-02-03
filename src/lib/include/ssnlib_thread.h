
#pragma once

#include <slankdev/exception.h>

namespace ssnlib {

class ssn_thread {
public:
    virtual void operator()() { printf("not set thread \n"); }
    virtual bool kill()
    {
        throw slankdev::exception("kill() is not implemented yet.");
    }
};

} /* namespace ssnlib */
