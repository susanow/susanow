

#pragma once

#include <ssnlib_thread.h>


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
