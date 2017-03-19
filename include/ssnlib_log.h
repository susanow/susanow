
#pragma once


template <class... ARGS>
inline void kernel_log(const char* fmt, ARGS... args)
{
    printf(fmt, args...);
}
