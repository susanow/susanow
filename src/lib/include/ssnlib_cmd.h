

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
 * @file ssnlib_cmd.h
 * @brief definition abstract command class
 * @author slankdev
 */


#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>


namespace ssnlib {


class Command {
    friend class Shell;
protected:
    const std::string name;
public:
    Command(const char* n) : name(n) {}
    virtual void operator()(const std::vector<std::string>& args) = 0;
    virtual ~Command() {}
};


// class Cmd_reboot
// class Cmd_show
//    show config
//    show route
//    show stats
//    show statistic
// class Cmd_port
//    port 0 linkdown
//    port 0 linkup
//    port 0 blink
// class Cmd_commit
// class Cmd_export
// class Cmd_inport






} /* namespace ssnlib */
