
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <string>

#include <slankdev/exception.h>
#include <slankdev/socketfd.h>
#include <slankdev/util.h>
#include <slankdev/asciicode.h>


class shell;
#include "command.h"
#include "keyfunction.h"


class shell {
private:
    std::string inputstr;
public:
    int fd;
    size_t cursor_index;

    bool closed; // XXX: erase  TODO
    const char* prompt;
    static std::vector<node*> commands;
    static std::vector<KeyFunc*> keyfuncs;
    static std::vector<std::string> history;
    size_t hist_index;

    shell() : fd(-1), cursor_index(0), closed(false), prompt(name()), hist_index(0) {}

    void close()
    {
        Printf("close shell\r\n");
    }
    void exec_command()
    {
        if (inputstr.empty()) return;

        ::printf("exec(\"%s\")\n", inputstr.c_str());
        for (node* c : commands) {
            node* nd = c->match(inputstr);
            if (nd) {
                nd->function(this);
                history.push_back(inputstr);
                ::printf("add_history \"%s\"\n", inputstr.c_str());
                clean_prompt();
                return ;
            }
        }
        Printf("command not found: \"%s\"\r\n", inputstr.c_str());
        clean_prompt();
    }
    void clean_prompt()
    {
        inputstr.clear();
        hist_index = 0;
        cursor_index = 0;
        Printf("\r%s%s", prompt);
    }
    void refresh_prompt()
    {
        char lineclear[] = {slankdev::AC_ESC, '[', 2, slankdev::AC_K};
        Printf("\r%s", lineclear);
        Printf("\r%s%s", prompt, inputstr.c_str());

        size_t backlen = inputstr.length() - cursor_index;
        char left [] = {slankdev::AC_ESC, '[', slankdev::AC_D};
        for (size_t i=0; i<backlen; i++) {
            Printf("%s", left);
        }
    }
    template <class... ARGS>
    void Printf(const char* fmt, ARGS... args)
    {
        FILE* fp = fdopen(fd, "w");
        ::fprintf(fp, fmt, args...);
        fflush(fp);
    }
    void buffer_clear()
    {
        inputstr.clear();
        cursor_index = 0;
    }
    const char* buffer_c_str() const
    {
        return inputstr.c_str();
    }
    size_t buffer_length() const
    {
        return inputstr.length();
    }
    void input_char_to_buffer(char c)
    {
        inputstr.insert(inputstr.begin() + cursor_index, c);
        cursor_index++;
    }
    void input_str_to_buffer(std::string& str)
    {
        for (char c : str) {
            input_char_to_buffer(c);
        }
    }
    void cursor_right() { cursor_index ++ ; }
    void cursor_left() { cursor_index -- ; }
    void cursor_backspace()
    {
        if (cursor_index > 0) {
            cursor_index --;
            printf("inputstr: \"%s\"\n", inputstr.c_str());
            printf("cursor idx: %zd\n", cursor_index);
            inputstr.erase(inputstr.begin() + cursor_index);
        }
    }
    void press_keys(const void* d, size_t l)
    {
        const uint8_t* p = reinterpret_cast<const uint8_t*>(d);
        if (l == 0) throw slankdev::exception("empty data received");

        for (KeyFunc* kf : keyfuncs) {
            if (kf->match(p, l)) {
                kf->function(this);
                return ;
            }
        }

        if (l > 1) {
            printf("Unsupport Escape Sequence\n");
            return ;
        }
        input_char_to_buffer(p[0]);
    }

public:

    const char* name()
    {
        static int c = 0;
        std::string* n = new std::string;
        *n = "Susanow" + std::to_string(c++) + "> ";
        return n->c_str();
    }

    int process()
    {
        char str[100];
        ssize_t res = ::read(fd, str, sizeof(str));
        if (res <= 0) return -1;

        press_keys(str, res);
        refresh_prompt();
        return 1;
    }

    void dispatch()
    {
        char str[] = "\r\n"
            "Hello, this is Susanow (version 0.00.00.0).\r\n"
            "Copyright 2017-2020 Hiroki SHIROKURA.\r\n"
            "\r\n";
        Printf(str);
        refresh_prompt();
    }
};
std::vector<node*> shell::commands;
std::vector<KeyFunc*> shell::keyfuncs;
std::vector<std::string> shell::history;


