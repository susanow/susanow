
#pragma once
#include <slankdev/vty.h>



static inline ssnlib::System* get_sys(slankdev::vty::shell* sh)
{
    return reinterpret_cast<ssnlib::System*>(sh->root_vty->user_ptr);
}



/*
 * show: SHOW SYSTEM INFORMATION
 *
 * ARGUMENT:
 * + author       : print author information
 * + version      : print version
 * + thread-info  : print thread information
 * + lthread-info : print lthread information
 * + port         : print port information
 * + cpu          : print cpu information
 */
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
    struct thread_info : public cmd_node {
        thread_info() : cmd_node("thread-info") {}
        void function(slankdev::vty::shell* sh)
        {
            sh->Printf(" %-4s %-20s %-10s \r\n", "No.", "Name", "Ptr");
            ssnlib::System* sys = get_sys(sh);
            size_t nb_threads = sys->threadpool.size();
            for (size_t i=0; i<nb_threads; i++) {
                const ssnlib::Thread* thread = sys->threadpool.get_thread(i);
                sh->Printf(" %-4zd %-20s %-10p \r\n",
                        i,
                        thread->name.c_str(),
                        thread);
            }
        }
    };
    struct lthread_info : public cmd_node {
        lthread_info() : cmd_node("lthread-info") {}
        void function(slankdev::vty::shell* sh)
        {
            sh->Printf(" %-4s %-20s %-10s \r\n", "No.", "Name", "Ptr");
            ssnlib::System* sys = get_sys(sh);

            size_t nb_threads = sys->ltsched.size();
            for (size_t i = 0; i<nb_threads; i++) {
                const ssnlib::slow_thread* thread = sys->ltsched.get_thread(i);
                sh->Printf(" %-4zd %-20s %-10p \r\n",
                        i,
                        thread->name.c_str(),
                        thread);
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



/*
 * halt: System halt Command
 * argument: none
 */
struct halt : public slankdev::vty::cmd_node {
    halt() : cmd_node("halt") {}
    void function(slankdev::vty::shell* sh)
    {
        sh->root_vty->halt();
    }
};


