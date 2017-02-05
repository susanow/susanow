

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
 * @file ssnlib_log.h
 * @brief include logger class
 * @author slankdev
 */

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
