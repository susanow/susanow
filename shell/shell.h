
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

#include "telnet.h"
#include "asciicode.h"


class shell {
    class Command {
    public:
        const std::string name;
        Command(const char* n) : name(n) {}
        virtual ~Command() {}
        virtual void exec() = 0;
    };

    class Cmd_quit : public Command {
        shell* sh;
    public:
        Cmd_quit(const char* str, shell* s) : Command(str), sh(s) {}
        void exec()
        {
            close(sh->fd);
            sh->fd = -1;
            sh->closed = true;
        }
    };
    class KeyFunc {
    public:
        const char code;
        KeyFunc(char c) : code(c) {}
        virtual ~KeyFunc() {}
        virtual void function() = 0;
    };
    class KF_question : public KeyFunc {
        shell* sh;
    public:
        KF_question(shell* s) : KeyFunc('?'), sh(s) {}
        void function() { printf("HATENA buf=\"%s\"\n", sh->inputstr.c_str()); }
    };
    class KF_return : public KeyFunc {
        shell* sh;
    public:
        KF_return(shell* s) : KeyFunc('\r'), sh(s) {}
        void function()
        {
            char cs[] = "\r\n";
            sh->write(cs, sizeof(cs));
            sh->exec_command();
            sh->refresh_promptline();
        }
    };
    class KF_delete : public KeyFunc {
        shell* sh;
    public:
        KF_delete(shell* s) : KeyFunc(0x7f), sh(s) {}
        void function()
        {
            char str[] = { 0x08, ' '};
            sh->write(str, sizeof(str));
            if (!sh->inputstr.empty()) {
                sh->inputstr.resize(sh->inputstr.length()-1);
                sh->refresh_promptline();
            }
        }
    };
    class KF_ctrl_B : public KeyFunc {
        shell* sh;
    public:
        KF_ctrl_B(shell* s) : KeyFunc(AC_Ctrl_B), sh(s) {}
        void function() { char c=0x08; sh->write(&c, 1); }
    };

private:
public:
    std::string inputstr;
    int fd;
    std::vector<Command*> commands;
    std::vector<KeyFunc*> keyfuncs;
    bool closed;

    void writestr(const char* str) { write(str, strlen(str)); }
    void write(const void* buf, size_t size) { ::write(fd, buf, size); }
    void init_keyfuncs()
    {
        keyfuncs.push_back(new KF_question(this));
        keyfuncs.push_back(new KF_return  (this));
        keyfuncs.push_back(new KF_delete  (this));
        keyfuncs.push_back(new KF_ctrl_B  (this));
    }
    void init_commands()
    {
        commands.push_back(new Cmd_quit("quit", this));
    }
    void exec_command()
    {
        if (inputstr.empty()) return;

        ::printf("exec(\"%s\")\n", inputstr.c_str());
        for (Command* c : commands) {
            if (c->name == inputstr) {
                c->exec();
            }
        }
        inputstr = "";
    }
    void press_key(char c)
    {
        for (KeyFunc* kf : keyfuncs) {
            if (kf->code == c) {
                kf->function();
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
        uint8_t right[] = {AC_ESC, '[', AC_C};
        uint8_t left [] = {AC_ESC, '[', AC_D};
        uint8_t up   [] = {AC_ESC, '[', AC_A};
        uint8_t down [] = {AC_ESC, '[', AC_B};

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

    void setup_telnet_term()
    {
        vty_will_echo (fd);
        vty_will_suppress_go_ahead (fd);
        vty_dont_linemode (fd);
        vty_do_window_size (fd);
    }
    void refresh_promptline()
    {
        write("\r", 1);
        write(prompt, strlen(prompt));
        write(inputstr.c_str(), inputstr.length());
    }
public:
    const char* prompt;
    shell() : fd(-1), closed(false), prompt("Susanow> ")
    {
        init_keyfuncs();
        init_commands();
    }

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
        setup_telnet_term();
        struct pollfd pfd;
        pfd.fd = fd;
        pfd.events = POLLIN | POLLERR;

        char str[] = "\r\n"
            "Hello, this is Susanow (version 0.00.00.0).\r\n"
            "Copyright 2017-2020 Hiroki SHIROKURA.\r\n"
            "\r\n";
        write(str, sizeof(str));
        refresh_promptline();
    }
};


