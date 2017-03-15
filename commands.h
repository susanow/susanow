
#pragma once
#include <slankdev/vty.h>
#include <slankdev/dpdk_struct.h>



static inline ssnlib::System* get_sys(slankdev::vty::shell* sh)
{
    return reinterpret_cast<ssnlib::System*>(sh->root_vty->user_ptr);
}

static inline std::string get_cpustate(uint32_t lcoreid)
{
    rte_lcore_state_t s = rte_eal_get_lcore_state(lcoreid);
    switch (s) {
        case WAIT    : return "WAIT    ";
        case RUNNING : return "RUNNING ";
        case FINISHED: return "FINISHED";
        default:
            throw slankdev::exception("UNKNOWN STATE");
    }
    return "FAAAA";
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
        thread_info() : cmd_node("thread-pool") {}
        void function(slankdev::vty::shell* sh)
        {
            const ssnlib::Thread* thread;
            size_t nb_threads;
            ssnlib::System* sys = get_sys(sh);
            sh->Printf("Threads\r\n");
            sh->Printf(" %-4s %-20s %-10s \r\n", "No.", "Name", "Ptr");
            thread = &sys->vty;
            sh->Printf(" %-4s %-20s %-10p \r\n", "N/A", thread->name.c_str(), thread);
            thread = &sys->ltsched;
            sh->Printf(" %-4s %-20s %-10p \r\n", "N/A", thread->name.c_str(), thread);
            nb_threads = sys->threadpool.size();
            for (size_t i=0; i<nb_threads; i++) {
                thread = sys->threadpool.get_thread(i);
                sh->Printf(" %-4zd %-20s %-10p \r\n",
                        i,
                        thread->name.c_str(),
                        thread);
            }

            sh->Printf("Lthreads\r\n");
            sh->Printf(" %-4s %-20s %-10s \r\n", "No.", "Name", "Ptr");
            nb_threads = sys->lthreadpool.size();
            for (size_t i = 0; i<nb_threads; i++) {
                const ssnlib::Lthread* thread = sys->lthreadpool.get_thread(i);
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
        void function(slankdev::vty::shell* sh)
        {
            sh->Printf(" %-4s %-5s %-5s %-10s \r\n", "ID", "rxQs", "txQs", "link" );
            ssnlib::System* sys = get_sys(sh);
            for (size_t i=0; i<sys->ports.size(); i++) {
                sh->Printf(" %-4zd %-5zd %-5zd\r\n",
                        i,
                        sys->ports[i].nb_rx_rings,
                        sys->ports[i].nb_tx_rings
                        );
                slankdev::print(&sys->ports[i].link.raw);
            }
        }
    };
    struct cpu : public cmd_node {
        cpu() : cmd_node("cpu") {}
        void function(slankdev::vty::shell* sh)
        {
            sh->Printf(" %-4s %-20s \r\n", "ID", "State");
            ssnlib::System* sys = get_sys(sh);
            for (size_t i=0; i<sys->cpus.size(); i++) {
                std::string state = get_cpustate(i);
                sh->Printf(" %-4zd %-20s \r\n", i, state.c_str());
            }
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


