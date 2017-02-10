

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
// #include "commands.h"
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

#if 0
    Shell shell;
    shell.add_cmd(new Cmd_clear   ("clear"               ));
    shell.add_cmd(new Cmd_quit    ("quit"  , &sys        ));
    shell.add_cmd(new Cmd_test    ("test"  , &sys, &shell));
    shell.add_cmd(new Cmd_run     ("run"   , &sys, &shell));
    shell.add_cmd(new Cmd_thread  ("thread", &sys        ));
    shell.add_cmd(new Cmd_show    ("show"  , &sys        ));
    shell.add_cmd(new Cmd_port    ("port"  , &sys        ));
    shell.add_cmd(new Cmd_findthread("find"  , &sys      ));
#else
    Shell shell("EthApp> ");
    shell.add_cmd(new quit);
    shell.add_cmd(new open);
    shell.add_cmd(new ip  );
    shell.fin();
    shell.interact();
#endif

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
    ssnt_wk wk(&sys, 25000);
    sys.append_thread(&timersubsys);
    sys.append_thread(&shell      );
    sys.append_thread(&rx         );
    sys.append_thread(&tx         );
    sys.append_thread(&wk         );
    sys.append_thread(&wk         );
    sys.append_thread(&wk         );
#endif

    // sys.cpus.at(1).launch();
    // sys.cpus.at(2).launch();
    // sys.cpus.at(3).launch();
    // sys.cpus.at(4).launch();
    // sys.cpus.at(5).launch();
    sys.launch_all();
    sys.wait_all();
}

