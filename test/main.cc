

#include <stdio.h>
#include <ssnlib_sys.h>
#include <ssnlib_shell.h>
#include <ssnlib_timer.h>
#include <ssnlib_thread.h>
#include <slankdev/system.h>

#if 1
using Rxq    = ssnlib::Rxq_interface<ssnlib::Ring_dpdk>;
using Txq    = ssnlib::Txq_interface<ssnlib::Ring_dpdk>;
#else
using Rxq    = ssnlib::Rxq_interface<ssnlib::Ring_stdqueue>;
using Txq    = ssnlib::Txq_interface<ssnlib::Ring_stdqueue>;
#endif
using Port   = ssnlib::Port_interface<Rxq, Txq>;
using Cpu    = ssnlib::Cpu_interface;
using System = ssnlib::System_interface<Cpu, Port>;
#include "commands.h"


#if 1
class ssnt_txrxwk : public ssnlib::ssn_thread {
    System* sys;
    bool running;
public:
    ssnt_txrxwk(System* s) : sys(s), running(false) {}
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
#endif
class ssnt_rx : public ssnlib::ssn_thread {
    System* sys;
    bool running;
public:
    ssnt_rx(System* s) : sys(s), running(false) {}
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
    ssnt_tx(System* s) : sys(s), running(false) {}
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
    ssnt_wk(System* s) : sys(s), running(false), nb_delay_clk(0) {}
    ssnt_wk(System* s, size_t d) : sys(s), running(false), nb_delay_clk(d) {}
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


class getportinfo : public ssnlib::Timer {
    System* sys;
    size_t rx_bps;
    size_t tx_bps;
public:
    getportinfo(System* s) : sys(s), rx_bps(0), tx_bps(0) {}
    void func() override
    {
        for (Port& port : sys->ports) {
            printf("port%u update \n", port.id);
            port.stats.update();
        }
    }
};

void func(struct rte_timer*, void* arg)
{
    System* sys = reinterpret_cast<System*>(arg);
    for (Port& port : sys->ports) {
        port.stats.update();
    }
    for (Port& port : sys->ports) {
        printf("port%u pps=%lu/%lu  bps=%lu/%lu \n", port.id,
                port.stats.rx_pps,
                port.stats.tx_pps,
                port.stats.rx_bps/1000000,
                port.stats.tx_bps/1000000
                );
    }
}


int main(int argc, char** argv)
{
    using namespace ssnlib;

    Port::nb_rx_rings    = 2;
    Port::nb_tx_rings    = 2;
    Port::rx_ring_size   = 128;
    Port::tx_ring_size   = 512;

    System sys(argc, argv);
    if (sys.ports.size()%2 != 0) return -1;

    Shell shell;
    shell.add_cmd(new Cmd_clear   ("clear"               ));
    shell.add_cmd(new Cmd_quit    ("quit"  , &sys        ));
    shell.add_cmd(new Cmd_test    ("test"  , &sys, &shell));
    shell.add_cmd(new Cmd_run     ("run"   , &sys, &shell));
    shell.add_cmd(new Cmd_thread  ("thread", &sys        ));
    shell.add_cmd(new Cmd_show    ("show"  , &sys        ));
    shell.add_cmd(new Cmd_port    ("port"  , &sys        ));


#if 1
    Timersubsys timersubsys;
    timersubsys.init();
    rte_timer timer;
    rte_timer_init(&timer);
    uint64_t hz = rte_get_timer_hz();
    rte_timer_reset(&timer, hz, PERIODICAL, 1, func, &sys);

    ssnt_txrxwk txrxwk(&sys);
    sys.cpus.at(1).thread = &timersubsys;
    sys.cpus.at(2).thread = &txrxwk;
    sys.cpus.at(3).thread = &shell;

    sys.cpus.at(1).launch();
    sys.cpus.at(2).launch();
    sys.cpus.at(3).launch();
#else
    ssnt_rx rx(&sys);
    ssnt_tx tx(&sys);
    ssnt_wk wk(&sys);
    sys.cpus.at(1).thread = &shell;
    sys.cpus.at(2).thread = &rx;
    sys.cpus.at(3).thread = &tx;
    sys.cpus.at(4).thread = &wk;
    sys.cpus.at(5).thread = &timersubsys;
    // sys.cpus.at(5).thread = &wk;
    // sys.cpus.at(6).thread = &wk;
    // sys.cpus.at(7).thread = &wk;
#endif

    sys.wait_all();
}

