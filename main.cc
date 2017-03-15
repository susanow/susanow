

volatile bool force_quit;
#include <stdio.h>
#include <string>
#include <ssnlib_sys.h>
#include <ssnlib_thread.h>
#include <ssnlib_log.h>
#include <slankdev/system.h>
#include <slankdev/filelogger.h>

#include "commands.h"
#include "threads.h"


std::string slankdev::filelogger::path = "syslog.out";
int main(int argc, char** argv)
{
    ssnlib::System sys(argc, argv);
    sys.vty.install_command(new halt);
    sys.vty.install_command(new show);

    sys.tthreadpool.add_thread(new timertest(&sys));
    sys.lthreadpool.add_thread(new slow_thread_test(0));
    sys.lthreadpool.add_thread(new slow_thread_test(1));
    sys.threadpool.add_thread(new txrxwk(&sys));

    size_t nb_ports = sys.ports.size();
    for (size_t i=0; i<nb_ports; i++) {
        sys.ports[i].init();
    }

    sys.dispatch();
    rte_eal_mp_wait_lcore();
}


