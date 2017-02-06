

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
#include "threads.h"
#include "timers.h"


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

    Timersubsys timersubsys(1);
    timersubsys.init();
    timersubsys.add_timer(new TimerDefault(&sys), rte_get_timer_hz());

#if 1
    ssnt_txrxwk txrxwk(&sys);
    sys.append_thread(&timersubsys);
    sys.append_thread(&txrxwk     );
    sys.append_thread(&shell      );
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

    sys.launch_all();
    sys.wait_all();
}

