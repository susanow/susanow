

#include <stdio.h>
#include <ssnlib_sys.h>
#include <ssnlib_shell.h>
#include <ssnlib_timer.h>
#include <ssnlib_thread.h>
#include <slankdev/system.h>

using Rxq    = ssnlib::Rxq_interface<ssnlib::Ring_dpdk>;
using Txq    = ssnlib::Txq_interface<ssnlib::Ring_dpdk>;
using Port   = ssnlib::Port_interface<Rxq, Txq>;
using Cpu    = ssnlib::Cpu_interface;
using System = ssnlib::System_interface<Cpu, Port>;
#include "commands.h"



class Allocator {
public:
    const std::string name;
    Allocator(const char* n) : name(n)
    { if (name == "") throw slankdev::exception("not set thread name"); }
    virtual ~Allocator() {}
    virtual ssnlib::ssn_thread* alloc() = 0;
};

class Allocator_wk : public Allocator {

    class Thrd_wk : public ssnlib::ssn_thread {
        bool running;
        size_t id;
    public:
        Thrd_wk() : ssn_thread("wk"), running(false) {}
        bool kill() { running = false; return true; }
        void operator()()
        {
            running = true;
            while (running) {
                sleep(1);
            }
        }
    };

public:
    Allocator_wk() : Allocator("wk") {}
    ssnlib::ssn_thread* alloc() override { return new Thrd_wk; }
};

class Allocator_rx : public Allocator {

    class Thrd_rx : public ssnlib::ssn_thread {
        bool running;
        size_t id;
    public:
        Thrd_rx() : ssn_thread("rx"), running(false) {}
        bool kill() { running = false; return true; }
        void operator()()
        {
            running = true;
            while (running) {
                sleep(1);
            }
        }
    };

public:
    Allocator_rx() : Allocator("rx") {}
    ssnlib::ssn_thread* alloc() override { return new Thrd_rx; }
};


class dta2 {

    class Cmd_dta : public ssnlib::Command {
        struct pcmd_params {
            cmdline_fixed_string_t cmd;
            cmdline_fixed_string_t op;
        };
        dta2* d;
    public:
        Cmd_dta(dta2* d_) : d(d_)
        {
            append_token(TOKEN_STRING(struct pcmd_params, cmd, "dta"));
            append_token(TOKEN_STRING(struct pcmd_params, op , NULL));
            token_fin();

            init_raw();
            set_raw("dta < inc** | dec** | stop >");
        }
        void handle(void* p)
        {
            pcmd_params* param = reinterpret_cast<pcmd_params*>(p);
            const std::string op = param->op;

            if (op == "stop") d->stop();
            else if (op == "incwk") d->inc("wk");
            else if (op == "incrx") d->inc("rx");
            else if (op == "decwk") d->dec("wk");
            else if (op == "decrx") d->dec("rx");
            else printf("Bad arguments\n");
        }
    };


public:
    System sys;
    ssnlib::Shell shell;
    std::vector<Allocator*> allocators;

public:
    dta2(int argc, char** argv) : sys(argc, argv), shell("susanow> ")
    {
        shell.add_cmd(new Cmd_clear           );
        shell.add_cmd(new Cmd_findthread(&sys));
        shell.add_cmd(new Cmd_quit      (&sys));
        shell.add_cmd(new Cmd_thread    (&sys));
        shell.add_cmd(new Cmd_port      (&sys));
        shell.add_cmd(new Cmd_lscpu     (&sys));
        shell.add_cmd(new Cmd_version   (&sys));
        shell.add_cmd(new Cmd_dta       (this));
        shell.fin();

        allocators.push_back(new Allocator_wk);
        allocators.push_back(new Allocator_rx);

        sys.append_thread(&shell);
    }
    void inc(const std::string& threadname)
    {
        try {
            for (auto* p : allocators) {
                if (p->name == threadname) {
                    sys.append_thread(p->alloc());
                    start();
                    return;
                }
            }
            printf("can't append thread\n");
        } catch (std::exception& e) {
            printf("can't append thread\n");
        }
    }
    void dec(const std::string& threadname)
    {
        size_t nb_cpus = sys.cpus.size();
        for (ssize_t i=nb_cpus-1; i>=0; i--) {
            if (i == 0) continue;

            Cpu& cpu = sys.cpus.at(i);
            if (cpu.thread && cpu.thread->name == threadname) {
                ssnlib::ssn_thread* tmp = cpu.thread;
                if (cpu.get_state() != RUNNING) {
                } else {
                    tmp->kill();
                    cpu.wait();
                }
                delete tmp;
                cpu.thread = nullptr;
                return ;
            }
        }
        printf("no such thread\n");
    }
    void start()
    {
        for (Cpu& cpu : sys.cpus) {
            if (cpu.thread && have_thread(cpu.thread->name)) {
                cpu.launch();
            }
        }
    }
    void stop()
    {
        for (Cpu& cpu : sys.cpus) {
            if (cpu.thread && have_thread(cpu.thread->name)) {
                cpu.thread->kill();
                cpu.wait();
            }
        }
    }
    bool have_thread(const std::string& name)
    {
        for (auto* alctr : allocators) {
            if (alctr->name == name) return true;
        }
        return false;
    }
};



int main(int argc, char** argv)
{
    using namespace ssnlib;

    Port::nb_rx_rings    = 2;
    Port::nb_tx_rings    = 2;
    Port::rx_ring_size   = 128;
    Port::tx_ring_size   = 512;

    dta2 d(argc, argv);

    d.sys.launch_all();
    d.sys.wait_all();
}

