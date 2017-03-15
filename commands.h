
#pragma once
#include <slankdev/vty.h>



class show : public slankdev::vty::cmd_node {
    struct author : public cmd_node {
        author() : cmd_node("author") {}
        void function(slankdev::vty::shell* sh)
        {
            sh->Printf("Hiroki SHIROKURA.\r\n");
            sh->Printf(" Twitter : @slankdev\r\n");
            sh->Printf(" Github  : slankdev\r\n");
            sh->Printf(" Facebook: hiroki.shirokura\r\n");
            sh->Printf(" E-mail  : slank.dev@gmail.com\r\n");
        }
    };
    struct lthread_info : public cmd_node {
        lthread_info() : cmd_node("lthread-info") {}
        void function(slankdev::vty::shell* sh)
        {
            sh->Printf("lthread info \r\n");
            ssnlib::System* sys = reinterpret_cast<ssnlib::System*>(sh->root_vty->user_ptr);

            size_t nb_threads = sys->ltsched.size();
            for (size_t i = 0; i<nb_threads; i++) {
                sh->Printf("slow_threads[%zd]: %p %s \r\n",
                        i,
                        sys->ltsched.get(i),
                        sys->ltsched.get(i)->name.c_str());
            }
        }
    };
    struct version : public cmd_node {
        version() : cmd_node("version") {}
        void function(slankdev::vty::shell* sh)
        {
            sh->Printf("Susanow 0.0.0\r\n");
            sh->Printf("Copyright 2017-2020 Hiroki SHIROKURA.\r\n");
        }
    };
    struct thread_info : public cmd_node {
        thread_info() : cmd_node("thread-info") {}
        void function(slankdev::vty::shell* sh)
        {
            sh->Printf("show thread-info\r\n");
            using namespace ssnlib;
            ssnlib::System* sys = reinterpret_cast<ssnlib::System*>(sh->root_vty->user_ptr);
            size_t nb_threads = sys->threadpool.size();
            for (size_t i=0; i<nb_threads; i++) {
                sh->Printf("thread[%d]: %s \r\n", i,
                        sys->threadpool.get_thread(i)->name.c_str());
            }
        }
    };
    struct port : public cmd_node {
        port() : cmd_node("port") {}
        void function(slankdev::vty::shell* sh) { sh->Printf("show port \r\n"); }
    };
    struct cpu : public cmd_node {
        cpu() : cmd_node("cpu") {}
        void function(slankdev::vty::shell* sh)
        {
            sh->Printf("show cpu \r\n");
        }
    };
public:
    show() : cmd_node("show")
    {
        commands.push_back(new author);
        commands.push_back(new version);
        commands.push_back(new thread_info);
        commands.push_back(new port);
        commands.push_back(new cpu);
        commands.push_back(new lthread_info);
    }
    void function(slankdev::vty::shell* sh) { sh->Printf("show\r\n"); }
};
struct halt : public slankdev::vty::cmd_node {
    halt() : cmd_node("halt") {}
    void function(slankdev::vty::shell* sh)
    {
        sh->root_vty->halt();
    }
};


