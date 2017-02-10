
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
 * @file ssnlib_shell.h
 * @brief include shell implementation
 * @author slankdev
 */


#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <memory>

#include <cmdline_parse.h>
#include <cmdline_parse_num.h>
#include <cmdline_parse_string.h>
#include <cmdline_parse_etheraddr.h>
#include <cmdline_socket.h>
#include <cmdline.h>

#include <slankdev/util.h>
#include <ssnlib_sys.h>
#include <ssnlib_thread.h>
#include <ssnlib_cmd.h>


namespace ssnlib {



struct Shell : public ssn_thread {
    struct ::cmdline* ctx_cmdline_;
    std::string p;
    std::vector<Command*> cmds;
    std::vector<cmdline_parse_ctx_t> ctx;

public:

    Shell(const char* prompt) : ssn_thread("shell"), p(prompt) {}
    ~Shell()
    {
        cmdline_stdin_exit(ctx_cmdline_);
        slankdev::vec_delete_all_ptr_elements<Command>(cmds);
    }
    void add_cmd(Command* cmd)
    {
        cmds.push_back(cmd);
        ctx.push_back(cmd->raw);
    }
    void fin() { ctx.push_back(nullptr); }
    void interact()
    {
        ctx_cmdline_ = cmdline_stdin_new(ctx.data(), p.c_str());
        cmdline_interact(ctx_cmdline_);
    }
    void operator()()
    {
        interact();
    }
};


} /* namespace ssnlib */
