
#pragma once

#include <slankdev/filelogger.h>

template <class... ARGS>
inline void kernel_log(const char* fmt, ARGS... args)
{
    slankdev::filelogger::get_instance().write(fmt, args...);
    printf(fmt, args...);
}
