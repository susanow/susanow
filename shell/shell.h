
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

    class Command {
    public:
        const std::string name;
        Command(const char* n) : name(n) {}
        virtual ~Command() {}
        virtual void exec(shell*) = 0;
    };
    class KeyFunc {
    public:
        const char code;
        KeyFunc(char c) : code(c) {}
        virtual ~KeyFunc() {}
        virtual void function(shell*) = 0;
    };


class shell {
public:

private:
public:
    static std::vector<Command*> commands;
    static std::vector<KeyFunc*> keyfuncs;

    void writestr(const char* str) { write(str, strlen(str)); }
    void write(const void* buf, size_t size) { ::write(fd, buf, size); }
    void exec_command()
    {
        if (inputstr.empty()) return;

        ::printf("exec(\"%s\")\n", inputstr.c_str());
        for (Command* c : commands) {
            if (c->name == inputstr) {
                c->exec(this);
            }
        }
        inputstr = "";
    }
    void press_key(char c)
    {
        for (KeyFunc* kf : keyfuncs) {
            if (kf->code == c) {
                kf->function(this);
                return ;
            }
        }
        inputstr += c;
        write(&c, 1);
    }
    void press_key_2(char str[], size_t len)
    {
        press_key(str[0]);
    }
    void press_key_3(char str[], size_t len)
    {
        uint8_t right[] = {slankdev::AC_ESC, '[', slankdev::AC_C};
        uint8_t left [] = {slankdev::AC_ESC, '[', slankdev::AC_D};
        uint8_t up   [] = {slankdev::AC_ESC, '[', slankdev::AC_A};
        uint8_t down [] = {slankdev::AC_ESC, '[', slankdev::AC_B};

        if (len != 3) return;
        if (memcmp(str, right, 3) == 0) cursor_right();
        else if (memcmp(str, left , 3) == 0) cursor_left ();
        else if (memcmp(str, up   , 3) == 0) cursor_up   ();
        else if (memcmp(str, down , 3) == 0) cursor_down ();
        else return;
    }
    void cursor_right() { writestr("\033[C"); }
    void cursor_left () { writestr("\033[D"); }
    void cursor_up   () { writestr("\033[A"); }
    void cursor_down () { writestr("\033[B"); }

    void refresh_promptline()
    {
        write("\r", 1);
        write(prompt, strlen(prompt));
        write(inputstr.c_str(), inputstr.length());
    }
public:
    int fd;
    std::string inputstr;
    bool closed;
    const char* prompt;

    const char* name()
    {
        static int c = 0;
        std::string* n = new std::string;
        *n = "Susanow" + std::to_string(c++) + "> ";
        return n->c_str();
    }
    shell() : fd(-1), closed(false), prompt(name()) {}

    int process()
    {
        char str[100];
        ssize_t res = ::read(fd, str, sizeof(str));
        if (res <= 0) return -1;

        if (res == 1 || res == 2) {
            press_key(str[0]);
        } else if (res == 2) {
            press_key_2(str, res);
        } else if (res == 3) {
            press_key_3(str, res);
        } else {
            return 1;
            printf("input control character length=%zd   [%x]\n", res, str[0]);
            slankdev::hexdump("", str, res);
            throw slankdev::exception("INPUT Long string");
        }
        return 1;
    }

    void dispatch()
    {
        char str[] = "\r\n"
            "Hello, this is Susanow (version 0.00.00.0).\r\n"
            "Copyright 2017-2020 Hiroki SHIROKURA.\r\n"
            "\r\n";
        write(str, sizeof(str));
        refresh_promptline();
    }
};
std::vector<Command*> shell::commands;
std::vector<KeyFunc*> shell::keyfuncs;



