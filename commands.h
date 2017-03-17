
#pragma once
#include <slankdev/vty.h>
#include <slankdev/dpdk_struct.h>
#include <ssnlib_thread.h>



static inline ssnlib::System* get_sys(slankdev::shell* sh)
{
    return reinterpret_cast<ssnlib::System*>(sh->user_ptr);
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



// class launch : public slankdev::vty::cmd_node {
//     struct fthread : public cmd_node {
//         fthread() : cmd_node("fthread") {}
//         void function(slankdev::vty::shell* sh)
//         {
//             ssnlib::System* sys = get_sys(sh);
//             sh->Printf("launch Fthread\r\n");
//             UNUSED(sys);
//         }
//     };
// public:
//     launch() : cmd_node("launch")
//     {
//         commands.push_back(new fthread);
//     }
//     void function(slankdev::vty::shell* sh)
//     {
//         ssnlib::System* sys = get_sys(sh);
//         UNUSED(sys);
//     }
// };


#if 0
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
#endif


struct echo : public slankdev::command {
    echo()
    {
        nodes.push_back(new slankdev::node_fixedstring("echo", "echoing user message"));
        nodes.push_back(new slankdev::node_string              );
    }
    void func(slankdev::shell* sh)
    {
        std::string s = nodes[1]->get();
        sh->Printf("%s\n", s.c_str());
    }
};

struct quit : public slankdev::command {
    quit() { nodes.push_back(new slankdev::node_fixedstring("quit", "quit system")); }
    void func(slankdev::shell* sh) { sh->close(); }
};

struct clear : public slankdev::command {
    clear() { nodes.push_back(new slankdev::node_fixedstring("clear", "clear screen")); }
    void func(slankdev::shell* sh) { sh->Printf("\033[2J\r\n"); }
};

struct list : public slankdev::command {
    list() { nodes.push_back(new slankdev::node_fixedstring("list", "list avalable commands")); }
    void func(slankdev::shell* sh)
    {
        const std::vector<slankdev::command*>& commands = *sh->commands;
        for (slankdev::command* cmd : commands) {
            std::string s = "";
            for (slankdev::node* nd : cmd->nodes) {
                s += nd->to_string();
                s += " ";
            }
            sh->Printf("  %s\r\n", s.c_str());
        }
    }
};

struct show_author : public slankdev::command {
    show_author()
    {
        nodes.push_back(new slankdev::node_fixedstring("show", "show infos"));
        nodes.push_back(new slankdev::node_fixedstring("author", "Author info"));
    }
    void func(slankdev::shell* sh)
    {
        sh->Printf("Hiroki SHIROKURA.\r\n");
        sh->Printf(" Twitter : @\r\n");
        sh->Printf(" Github  : \r\n");
        sh->Printf(" Facebook: hiroki.shirokura\r\n");
        sh->Printf(" E-mail  : slank.dev@gmail.com\r\n");
    }
};

struct show_version : public slankdev::command {
    show_version()
    {
        nodes.push_back(new slankdev::node_fixedstring("show", "show infos"));
        nodes.push_back(new slankdev::node_fixedstring("version", "Print version"));
    }
    void func(slankdev::shell* sh)
    {
        sh->Printf("Susanow 0.0.0\r\n");
        sh->Printf("Copyright 2017-2020 Hiroki SHIROKURA.\r\n");
    }
};

class show_thread_info : public slankdev::command {
public:
    show_thread_info()
    {
        nodes.push_back(new slankdev::node_fixedstring("show", "show infos"));
        nodes.push_back(new slankdev::node_fixedstring("thread-info", "thread information"));
    }
    std::string get_launch_state(ssnlib::System* sys, const ssnlib::Fthread* thread)
    {
        const size_t nb_cpus = sys->cpus.size();
        for (size_t i=1; i<nb_cpus; i++) {
            if (sys->cpus[i].thread == thread) {
                return "lcore" + std::to_string(i);
            }
        }
        return "none";
    }
    void func(slankdev::shell* sh)
    {
        const ssnlib::Fthread* thread;
        size_t nb_threads;
        std::string state;
        ssnlib::System* sys = get_sys(sh);
        sh->Printf(" Fthreads\r\n");
        sh->Printf("   %-4s %-20s %-20s %-10s\r\n", "No.", "Name", "Ptr", "State");
        thread = &sys->vty;
        state = get_launch_state(sys, thread);
        sh->Printf("   %-4s %-20s %-20p %-10s\r\n",
                "N/A", thread->name.c_str(), thread, state.c_str());
        thread = &sys->ltsched;
        state = get_launch_state(sys, thread);
        sh->Printf("   %-4s %-20s %-20p %-10s\r\n",
                "N/A", thread->name.c_str(), thread, state.c_str());
        nb_threads = sys->fthreadpool.size();
        for (size_t i=0; i<nb_threads; i++) {
            thread = sys->fthreadpool.get_thread(i);
            state = get_launch_state(sys, thread);
            sh->Printf("   %-4zd %-20s %-20p %-10s\r\n",
                    i,
                    thread->name.c_str(),
                    thread,
                    state.c_str());
        }

        sh->Printf(" Lthreads\r\n");
        sh->Printf("   %-4s %-20s %-10s \r\n", "No.", "Name", "Ptr");
        nb_threads = sys->lthreadpool.size();
        for (size_t i = 0; i<nb_threads; i++) {
            const ssnlib::Lthread* thread = sys->lthreadpool.get_thread(i);
            sh->Printf("   %-4zd %-20s %-10p \r\n",
                    i,
                    thread->name.c_str(),
                    thread);
        }

        sh->Printf(" Tthreads\r\n");
        sh->Printf("   %-4s %-20s %-10s \r\n", "No.", "Name", "Ptr");
        nb_threads = sys->tthreadpool.size();
        for (size_t i = 0; i<nb_threads; i++) {
            const ssnlib::Tthread* thread = sys->tthreadpool.get_thread(i);
            sh->Printf("   %-4zd %-20s %-10p \r\n",
                    i,
                    thread->name.c_str(),
                    thread);
        }
    }
};

class show_port : public slankdev::command {
public:
    show_port()
    {
        nodes.push_back(new slankdev::node_fixedstring("show", "show infos"));
        nodes.push_back(new slankdev::node_fixedstring("port", "port info"));
    }
    void func(slankdev::shell* sh)
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

class show_cpu : public slankdev::command {
public:
    show_cpu()
    {
        nodes.push_back(new slankdev::node_fixedstring("show", "show infos"));
        nodes.push_back(new slankdev::node_fixedstring("cpu", "cpu infos"));
    }
    void func(slankdev::shell* sh)
    {
        sh->Printf(" %-4s %-10s %-20s %-20s\r\n", "ID", "State", "Thread", "Name");
        ssnlib::System* sys = get_sys(sh);
        for (size_t i=0; i<sys->cpus.size(); i++) {
            std::string state = get_cpustate(i);
            ssnlib::Fthread* thread = sys->cpus[i].thread;
            sh->Printf(" %-4zd %-10s %-20p %-20s\r\n", i, state.c_str(),
                    thread, thread?thread->name.c_str():"none");
        }
    }
};


