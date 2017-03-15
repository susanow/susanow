
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
}


class launch : public slankdev::vty::cmd_node {
    struct fthread : public cmd_node {
        fthread() : cmd_node("fthread") {}
        void function(slankdev::vty::shell* sh)
        {
            ssnlib::System* sys = get_sys(sh);
            sh->Printf("launch Fthread\r\n");
            UNUSED(sys);
        }
    };
    struct lthread : public cmd_node {
        lthread() : cmd_node("lthread") {}
        void function(slankdev::vty::shell* sh)
        {
            ssnlib::System* sys = get_sys(sh);
            sh->Printf("launch Lthread\r\n");
            UNUSED(sys);
        }
    };
    struct tthread : public cmd_node {
        tthread() : cmd_node("tthread") {}
        void function(slankdev::vty::shell* sh)
        {
            ssnlib::System* sys = get_sys(sh);
            sh->Printf("launch Tthread\r\n");
            UNUSED(sys);
        }
    };
public:
    launch() : cmd_node("launch")
    {
        commands.push_back(new fthread);
        commands.push_back(new lthread);
        commands.push_back(new tthread);
    }
    void function(slankdev::vty::shell* sh)
    {
        ssnlib::System* sys = get_sys(sh);
        UNUSED(sys);
    }
};

class kill : public slankdev::vty::cmd_node {
    struct fthread : public cmd_node {
        fthread() : cmd_node("fthread") {}
        void function(slankdev::vty::shell* sh)
        {
            ssnlib::System* sys = get_sys(sh);
            sh->Printf("kill Fthread\r\n");
            UNUSED(sys);
        }
    };
    struct lthread : public cmd_node {
        lthread() : cmd_node("lthread") {}
        void function(slankdev::vty::shell* sh)
        {
            ssnlib::System* sys = get_sys(sh);
            sh->Printf("kill Lthread\r\n");
            UNUSED(sys);
        }
    };
    struct tthread : public cmd_node {
        tthread() : cmd_node("tthread") {}
        void function(slankdev::vty::shell* sh)
        {
            ssnlib::System* sys = get_sys(sh);
            sh->Printf("kill Tthread\r\n");
            UNUSED(sys);
        }
    };
public:
    kill() : cmd_node("kill")
    {
        commands.push_back(new fthread);
        commands.push_back(new lthread);
        commands.push_back(new tthread);
    }
    void function(slankdev::vty::shell* sh)
    {
        ssnlib::System* sys = get_sys(sh);
        UNUSED(sys);
    }
};


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
            const ssnlib::Fthread* thread;
            size_t nb_threads;
            ssnlib::System* sys = get_sys(sh);
            sh->Printf("Fthreads\r\n");
            sh->Printf(" %-4s %-20s %-10s \r\n", "No.", "Name", "Ptr");
            thread = &sys->vty;
            sh->Printf(" %-4s %-20s %-10p \r\n", "N/A", thread->name.c_str(), thread);
            thread = &sys->ltsched;
            sh->Printf(" %-4s %-20s %-10p \r\n", "N/A", thread->name.c_str(), thread);
            nb_threads = sys->fthreadpool.size();
            for (size_t i=0; i<nb_threads; i++) {
                thread = sys->fthreadpool.get_thread(i);
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

            sh->Printf("Tthreads\r\n");
            sh->Printf(" %-4s %-20s %-10s \r\n", "No.", "Name", "Ptr");
            nb_threads = sys->tthreadpool.size();
            for (size_t i = 0; i<nb_threads; i++) {
                const ssnlib::Tthread* thread = sys->tthreadpool.get_thread(i);
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
            ssnlib::System* sys = get_sys(sh);
            for (size_t i=0; i<sys->ports.size(); i++) {
                auto& port = sys->ports.at(i);
                sh->Printf("  Port id      : %zd\r\n", i);
                sh->Printf("  Link speed   : %u Mbps\r\n",
                        port.link.raw.link_speed);
                sh->Printf("  Link duplex  : %s\r\n",
                        port.link.raw.link_duplex==0?"Half":"Full");
                sh->Printf("  Link autoneg : %s\r\n",
                        port.link.raw.link_autoneg==0?"Fixed":"Autoneg");
                sh->Printf("  Link status  : %s\r\n",
                        port.link.raw.link_status==0?"Down":"Up");
                sh->Printf("  NB Tx Queue  : %zd\r\n", port.nb_rx_rings);
                sh->Printf("  Tx Queue size: %zd\r\n", port.rx_ring_size);
                sh->Printf("  NB Rx Queue  : %zd\r\n", port.nb_tx_rings);
                sh->Printf("  Rx Queue size: %zd\r\n", port.tx_ring_size);
                sh->Printf("  Mbps Rx/Tx   : %zd/%zd\r\n",
                    port.stats.rx_bps/1000000, port.stats.tx_bps/1000000);
                sh->Printf("  Kpps Rx/Tx   : %zd/%zd \r\n",
                    port.stats.rx_pps/1000   , port.stats.tx_pps/1000);
                sh->Printf("\r\n");
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


