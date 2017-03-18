

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
    sys.vty.install_command(new quit            );
    sys.vty.install_command(new clear           );
    sys.vty.install_command(new echo            );
    sys.vty.install_command(new list            );
    sys.vty.install_command(new show_author     );
    sys.vty.install_command(new show_version    );
    sys.vty.install_command(new show_cpu        );
    sys.vty.install_command(new show_port       );
    sys.vty.install_command(new show_thread_info);
    sys.vty.install_command(new launch_fthread  );
    sys.vty.install_command(new find_fthread    );

    sys.tthreadpool.add_thread(new timertest(&sys));

    sys.lthreadpool.add_thread(new slow_thread_test(0));
    sys.lthreadpool.add_thread(new slow_thread_test(1));
    sys.lthreadpool.add_thread(new slow_thread_test(2));
    sys.lthreadpool.add_thread(new slow_thread_test(3));
    sys.lthreadpool.add_thread(new slow_thread_test(4));

    sys.fthreadpool.add_thread(new txrxwk(&sys));

    sys.dispatch();
    rte_eal_mp_wait_lcore();
}



