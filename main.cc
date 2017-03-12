

#include <stdio.h>
#include <ssnlib_sys.h>
#include <ssnlib_shell.h>
#include <ssnlib_timer.h>
#include <ssnlib_thread.h>
#include <slankdev/system.h>
#include <slankdev/filelogger.h>

using Rxq    = ssnlib::Rxq_interface<ssnlib::Ring_dpdk>;
using Txq    = ssnlib::Txq_interface<ssnlib::Ring_dpdk>;
using Port   = ssnlib::Port_interface<Rxq, Txq>;
using Cpu    = ssnlib::Cpu_interface;
using System = ssnlib::System_interface<Cpu, Port>;
#include "commands.h"


using slankdev::filelogger;
std::string filelogger::path = "log.out";

int main(int argc, char** argv)
{
    // using namespace ssnlib;
    //
    // Port::nb_rx_rings    = 2;
    // Port::nb_tx_rings    = 2;
    // Port::rx_ring_size   = 128;
    // Port::tx_ring_size   = 512;
    //
    // dta2system sys(argc, argv);
    //
    // ssnlib::Shell shell("susanow> ");
    // shell.fin();
    //
    // sys.append_thread(&shell);
    // sys.append_dtaa_thread(new Allocator_wk);
    // sys.append_dtaa_thread(new Allocator_rx);
    //
    // sys.launch_all();
    // sys.wait_all();
}

