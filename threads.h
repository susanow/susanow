
#pragma once

#include <ssnlib_sys.h>
#include <ssnlib_thread.h>
#include <slankdev/string.h>




class timertest : public ssnlib::Tthread {
    ssnlib::System* sys;
public:
    timertest(ssnlib::System* s) : Tthread("timertest"), sys(s) {}
    void impl()
    {
        for (ssnlib::Port& port : sys->ports) {
            port.stats.update();
            port.link.update();
        }
    }
};



struct slow_thread_test : public ssnlib::Lthread {
    int a;
    slow_thread_test(int b) : Lthread(
            slankdev::fs("slowthread(%d)", b).c_str()
            ), a(b) {}
    void impl()
    {
        printf("impl %d\n", a);
        sleep(1);
    }
};


class txrxwk : public ssnlib::Fthread {
    ssnlib::System* sys;
public:
    txrxwk(ssnlib::System* s) : Fthread("txrxwk"), sys(s) {}
    void impl()
    {
        const uint8_t nb_ports = sys->ports.size();
        while (true) {
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
};



