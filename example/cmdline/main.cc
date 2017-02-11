

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

    sys.append_thread(&shell      );
    sys.launch_all();
    sys.wait_all();
}

