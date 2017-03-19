


/*-
 * MIT License
 *
 * Copyright (c) 2017 Susanow
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/**
 * @file main.cc
 * @author slankdev
 */


#include <stdio.h>
#include <string>
#include <ssnlib_sys.h>
#include <ssnlib_thread.h>
#include <ssnlib_log.h>
#include <slankdev/system.h>
#include <slankdev/filelogger.h>

#include "commands.h"
#include "threads.h"


void install_vty_commands(System* sys)
{
    sys->vty.install_command(new quit            );
    sys->vty.install_command(new clear           );
    sys->vty.install_command(new echo            );
    sys->vty.install_command(new list            );
    sys->vty.install_command(new show_author     );
    sys->vty.install_command(new show_version    );
    sys->vty.install_command(new show_cpu        );
    sys->vty.install_command(new show_port       );
    sys->vty.install_command(new show_thread_info);
    sys->vty.install_command(new find_fthread    );
    sys->vty.install_command(new launch_fthread  );
    sys->vty.install_command(new kill_fthread    );
}


void init_thread_pool(System* sys)
{
    sys->tthreadpool.add_thread(new timertest(sys));
    sys->lthreadpool.add_thread(new slow_thread_test(0));
    sys->lthreadpool.add_thread(new slow_thread_test(1));
    sys->lthreadpool.add_thread(new slow_thread_test(2));
    sys->lthreadpool.add_thread(new slow_thread_test(3));
    sys->lthreadpool.add_thread(new slow_thread_test(4));
    sys->fthreadpool.add_thread(new txrxwk(sys));
}


std::string slankdev::filelogger::path = "syslog.out";
int main(int argc, char** argv)
{
    System sys(argc, argv);
    install_vty_commands(&sys);
    init_thread_pool(&sys);
    sys.dispatch();
}



