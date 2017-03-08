

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
#include <string.h>

#include <vector>
#include <string>

/*
 * For new version
 */
#include <cmdline_parse.h>
#include <cmdline_parse_num.h>
#include <cmdline_parse_string.h>
#include <cmdline_parse_etheraddr.h>
#include <cmdline_socket.h>
#include <cmdline.h>

namespace ssnlib {

#define TOKEN_STRING TOKEN_STRING_INITIALIZER
#define TOKEN_NUMBER TOKEN_NUM_INITIALIZER
using token_str = cmdline_parse_token_string_t;
using token_num = cmdline_parse_token_num_t;

#if 0
class Command {
    friend class Shell;
protected:
    const std::string name;
public:
    Command(const char* n) : name(n) {}
    virtual void operator()(const std::vector<std::string>& args) = 0;
    virtual ~Command() {}
};
#else
struct Command {
    std::vector<void*> tokens;
    cmdline_parse_inst_t* raw;

    Command() : raw(nullptr) {}
    virtual ~Command()
    {
        if (raw) free(raw);
        while (tokens.size() > 0) {
            void* tmp = tokens.back();
            tokens.pop_back();
            free(tmp);
        }
    }
    virtual void handle(void*) = 0;
    static void callback(void* param, struct cmdline*, void* data)
    {
        Command* cmd = reinterpret_cast<Command*>(data);
        cmd->handle(param);
    }
    void init_raw()
    {
        size_t size = 3 * sizeof(void*);
        size += sizeof(void*) * tokens.size();
        raw = (cmdline_parse_inst_t*)malloc(size);
    }
    void set_raw(const char* hs)
    {
        raw->data = this;
        raw->help_str = hs;
        raw->f = Command::callback;
        memcpy(&raw->tokens, tokens.data(), tokens.size()*sizeof(void*));
    }
    void append_token(token_str TT)
    {
        token_str* T = (token_str*)malloc(sizeof(token_str));
        *T = TT;
        tokens.push_back(T);
    }
    void append_token(token_num TT)
    {
        token_num* T = (token_num*)malloc(sizeof(token_num));
        *T = TT;
        tokens.push_back(T);
    }
    void token_fin() { tokens.push_back(nullptr); }

};
#endif


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
