

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





class Cmd_drcp : public ssnlib::Command {

    class Thrd_wk : public ssnlib::ssn_thread {
        System* sys;
        bool running;
        size_t id;
    public:
        Thrd_wk(System* s) : ssn_thread("drcp_thread"), sys(s), running(false)
        {
            static size_t cnt = 0;
            id = cnt;
            cnt++;
        }
        bool kill() { running = false; return true; }
        void operator()()
        {
            running = true;
            while (running) {
                printf("%zd", id);
                fflush(stdout);
                sleep(1);
            }
        }
    };

private:
    struct pcmd_params {
        cmdline_fixed_string_t cmd;
        cmdline_fixed_string_t op;
    };
    System* sys;
    const std::string threadname;
public:
    Cmd_drcp(System* s) : sys(s), threadname("drcp_thread")
    {
        append_token(TOKEN_STRING(struct pcmd_params, cmd, "drcp"));
        append_token(TOKEN_STRING(struct pcmd_params, op , NULL  ));
        token_fin();

        init_raw();
        set_raw("drcp < inc | dec | start | stop >: drcp control");
    }
    void inc()
    {
        try {
            sys->append_thread(new Thrd_wk(sys));
            start();
        } catch (std::exception& e) {
            printf("can't append thread\n");
        }
    }
    void dec()
    {
        size_t nb_cpus = sys->cpus.size();
        for (ssize_t i=nb_cpus-1; i>=0; i--) {
            if (i == 0) continue;

            Cpu& cpu = sys->cpus.at(i);
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
        for (Cpu& cpu : sys->cpus) {
            if (cpu.thread && cpu.thread->name==threadname) {
                cpu.launch();
            }
        }
    }
    void stop()
    {
        for (Cpu& cpu : sys->cpus) {
            if (cpu.thread && cpu.thread->name==threadname) {
                cpu.thread->kill();
                cpu.wait();
            }
        }
    }
    void handle(void* p)
    {
        pcmd_params* param = reinterpret_cast<pcmd_params*>(p);
        std::string op = param->op;

        if (op == "inc")        inc();
        else if (op == "dec"  ) dec();
        else if (op == "start") start();
        else if (op == "stop" ) stop();
        else printf("Bad Arguments \n");
    }
};



int main(int argc, char** argv)
{
    using namespace ssnlib;

    Port::nb_rx_rings    = 2;
    Port::nb_tx_rings    = 2;
    Port::rx_ring_size   = 128;
    Port::tx_ring_size   = 512;

    System sys(argc, argv);
    if (sys.ports.size()%2 != 0) return -1;

    Shell shell("susanow> ");
    shell.add_cmd(new Cmd_clear           );
    shell.add_cmd(new Cmd_findthread(&sys));
    shell.add_cmd(new Cmd_quit      (&sys));
    shell.add_cmd(new Cmd_thread    (&sys));
    shell.add_cmd(new Cmd_port      (&sys));
    shell.add_cmd(new Cmd_lscpu     (&sys));
    shell.add_cmd(new Cmd_version   (&sys));
    shell.add_cmd(new Cmd_drcp      (&sys));
    shell.fin();

    sys.append_thread(&shell);
    sys.launch_all();
    sys.wait_all();
}

