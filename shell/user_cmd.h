
#pragma once


struct Cmd_show : public node {
    struct author : public node {
        author() : node("author") {}
        void function(shell* sh)
        {
            sh->Printf("Hiroki SHIROKURA.\r\n");
            sh->Printf(" Twitter : @slankdev\r\n");
            sh->Printf(" Github  : slankdev\r\n");
            sh->Printf(" Facebook: hiroki.shirokura\r\n");
            sh->Printf(" E-mail  : slank.dev@gmail.com\r\n");
        }
    };
    struct version : public node {
        version() : node("version") {}
        void function(shell* sh)
        {
            sh->Printf("Susanow 0.0.0\r\n");
            sh->Printf("Copyright 2017-2020 Hiroki SHIROKURA.\r\n");
        }
    };
    struct thread_info : public node {
        thread_info() : node("thread-info") {}
        void function(shell* sh)
        {
            sh->Printf("show thread-info\r\n");
        }
    };
    Cmd_show() : node("show")
    {
        commands.push_back(new author);
        commands.push_back(new version);
        commands.push_back(new thread_info);
    }
    void function(shell* sh) { sh->Printf("show\r\n"); }
};

struct Cmd_quit : public node {
    Cmd_quit() : node("quit") {}
    void function(shell* sh)
    {
        sh->Printf("quit system\r\n");
        sh->closed = true;
    }
};

