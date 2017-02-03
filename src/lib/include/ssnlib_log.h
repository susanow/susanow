

#pragma once

#include <stdio.h>



namespace ssnlib {


enum LOG_LEVEL {
    SYSTEM,
    DEBUG,
    ERROR,
    SLANKDEV,
};


inline const char* LOG_LEVEL2str(LOG_LEVEL l)
{
    switch (l) {
        case SYSTEM  : return "SYS"  ;
        case DEBUG   : return "DEBUG";
        case ERROR   : return "ERR";
        case SLANKDEV: return "SLANKDEV";
        default: return "UNKNOWN_ERROR";
    }
}



template <class... Args>
inline void kernel_log(LOG_LEVEL level, const char* fmt, Args... args)
{
    printf("%s: ", LOG_LEVEL2str(level));
    printf(fmt, args...);
}



} /* namespace ssnlib */
