

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


class ssnt_txrxwk : public ssnlib::ssn_thread {
    System* sys;
    bool running;
public:
    ssnt_txrxwk(System* s) : ssn_thread("txrxwk"), sys(s), running(false) {}
    void operator()()
    {
        const uint8_t nb_ports = sys->ports.size();
        running = true;
        while (running) {
            for (uint8_t pid = 0; pid < nb_ports; pid++) {
                uint8_t nb_rxq = sys->ports[pid].rxq.size();
                uint8_t nb_txq = sys->ports[pid].txq.size();
                assert(nb_txq == nb_rxq);

                for (uint8_t qid=0; qid<nb_rxq; qid++) {
                    auto& in_port  = sys->ports[pid];
                    auto& out_port = sys->ports[pid^1];

                    in_port.rxq[qid].burst_bulk();

                    const size_t burst_size = 32;
                    rte_mbuf* pkts[burst_size];
                    bool ret = in_port.rxq[qid].pop_bulk(pkts, burst_size);
                    if (ret) out_port.txq[qid].push_bulk(pkts, burst_size);

                    out_port.txq[qid].burst_bulk();
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

    ssnt_txrxwk txrxwk(&sys);
    sys.append_thread(&timersubsys);
    sys.append_thread(&txrxwk     );
    sys.append_thread(&shell      );

    sys.launch_all();
    sys.wait_all();
}

