

#include <stdio.h>
#include <string>
#include <ssnlib_sys.h>
#include <ssnlib_thread.h>
#include <ssnlib_log.h>
#include <slankdev/system.h>
#include <slankdev/filelogger.h>

volatile bool force_quit;
#include "commands.h"
#include "threads.h"


int ff(void*)
{
    for (int i=0; i<5; i++) {
        sleep(1);
        printf("sleep %d \n", i);
        fflush(stdout);
    }
    force_quit = true;
    for (int i=0; i<5; i++) {
        sleep(1);
        printf("sleep %d \n", i);
        fflush(stdout);
    }
    return 0;
}


std::string slankdev::filelogger::path = "syslog.out";
int main(int argc, char** argv)
{
    ssnlib::System sys(argc, argv);
    sys.threads.push_back(new vty_thread);
    sys.threads.push_back(new lthread_sched);

    rte_eal_remote_launch(ssnlib::_thread_launch, sys.threads[0], 1);
    rte_eal_remote_launch(ssnlib::_thread_launch, sys.threads[1], 2);
    rte_eal_remote_launch(ff, NULL, 3);
    rte_eal_mp_wait_lcore();
}



