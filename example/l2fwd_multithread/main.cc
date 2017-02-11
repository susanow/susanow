

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



class ssnt_rx : public ssnlib::ssn_thread {
    System* sys;
    bool running;
public:
    ssnt_rx(System* s) : ssn_thread("rx"), sys(s), running(false) {}
    void operator()()
    {
        const uint8_t nb_ports = sys->ports.size();
        running = true;
        while (running) {
            for (uint8_t pid = 0; pid < nb_ports; pid++) {
                uint8_t nb_rxq = sys->ports[pid].rxq.size();
                for (uint8_t qid=0; qid<nb_rxq; qid++) {
                    sys->ports[pid].rxq[qid].burst_bulk();
                }
            }
        }
    }
    bool kill() { running=false; return true; }
};

class ssnt_tx : public ssnlib::ssn_thread {
    System* sys;
    bool running;
public:
    ssnt_tx(System* s) : ssn_thread("tx"), sys(s), running(false) {}
    void operator()()
    {
        const uint8_t nb_ports = sys->ports.size();
        running = true;
        while (running) {
            for (uint8_t pid = 0; pid < nb_ports; pid++) {
                uint8_t nb_txq = sys->ports[pid].txq.size();
                for (uint8_t qid=0; qid<nb_txq; qid++) {
                    sys->ports[pid].txq[qid].burst_bulk();
                }
            }
        }
    }
    bool kill() { running=false; return true; }
};

class ssnt_wk : public ssnlib::ssn_thread {
    System* sys;
    bool running;
    size_t nb_delay_clk;
public:
    ssnt_wk(System* s) : ssn_thread("wk"), sys(s), running(false), nb_delay_clk(0) {}
    ssnt_wk(System* s, size_t d) : ssn_thread("wk"), sys(s), running(false), nb_delay_clk(d) {}
    void operator()()
    {
        const uint8_t nb_ports = sys->ports.size();
        running = true;
        while (running) {
            for (uint8_t pid = 0; pid < nb_ports; pid++) {
                uint8_t nb_rxq = sys->ports[pid].rxq.size();
                uint8_t nb_txq = sys->ports[pid].txq.size();
                assert(nb_rxq == nb_txq);

                for (uint8_t qid=0; qid<nb_rxq; qid++) {
                    auto& in_port  = sys->ports[pid];
                    auto& out_port = sys->ports[pid^1];

                    const size_t burst_size = 32;
                    rte_mbuf* pkts[burst_size];
                    bool ret = in_port.rxq[qid].pop_bulk(pkts, burst_size);
                    if (ret) {
                        slankdev::delay_clk(nb_delay_clk);
                        out_port.txq[qid].push_bulk(pkts, burst_size);
                    }
                }
            }
        }
    }
    bool kill() { running=false; return true; }
};


class TimerDefault : public ssnlib::Timer {
    System* sys;
public:
    TimerDefault(System* s) : sys(s) {}
    ~TimerDefault() {}
    void func() override
    {
        sys->cyclic_task();
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
    shell.fin();

    Timersubsys timersubsys(1);
    timersubsys.init();
    timersubsys.add_timer(new TimerDefault(&sys), rte_get_timer_hz());

    ssnt_rx rx(&sys);
    ssnt_tx tx(&sys);
    ssnt_wk wk(&sys);
    sys.append_thread(&timersubsys);
    sys.append_thread(&shell      );
    sys.append_thread(&rx         );
    sys.append_thread(&tx         );
    sys.append_thread(&wk         );

    sys.launch_all();
    sys.wait_all();
}

