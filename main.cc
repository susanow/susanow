


/*-
 * MIT License
 *
 * Copyright (c) 2017 Hiroki SHIROKURA
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

#include "command/misc.h"
#include "command/port.h"
#include "command/thread.h"

#include "thread/wk.h"


void install_vty_commands(System* sys);
void init_thread_pool(System* sys);


int main(int argc, char** argv)
{
  System sys(argc, argv);
  install_vty_commands(&sys);
  init_thread_pool(&sys);
  sys.dispatch();
}



void install_vty_commands(System* sys)
{
  /*
   * Misc Commands
   */
  sys->vty.install_command(new quit            );
  sys->vty.install_command(new clear           );
  sys->vty.install_command(new echo            );
  sys->vty.install_command(new list            );
  sys->vty.install_command(new show_author     );
  sys->vty.install_command(new show_version    );
  sys->vty.install_command(new show_cpu        );
  sys->vty.install_command(new show_port       );
  sys->vty.install_command(new show_thread_info);

  /*
   * Port Commands
   */
  sys->vty.install_command(new port_configure  );
  sys->vty.install_command(new port_set_nbq    );
  sys->vty.install_command(new port_link_down  );
  sys->vty.install_command(new port_link_up    );
  sys->vty.install_command(new port_dev_start  );
  sys->vty.install_command(new port_dev_stop   );
  sys->vty.install_command(new port_rss        );
  sys->vty.install_command(new port_rxmode_show);
  sys->vty.install_command(new port_statistics );

  /*
   * Fthread Commands
   */
  sys->vty.install_command(new fthread_list    );
  sys->vty.install_command(new fthread_find    );
  sys->vty.install_command(new fthread_kill    );
  sys->vty.install_command(new fthread_launch  );

  /*
   * Lthread Commands
   */
  sys->vty.install_command(new lthread_list    );
  sys->vty.install_command(new lthread_find    );
  sys->vty.install_command(new lthread_kill    );
  sys->vty.install_command(new lthread_launch  );
  sys->vty.install_command(new lthread_scheduler_show);
}



void init_thread_pool(System* sys)
{
  sys->tthreadpool.add_thread(new timertest(sys) );

  // sys->lthreadpool.add_thread(new lthread_test(0));
  // sys->lthreadpool.add_thread(new lthread_test(1));
  // sys->lthreadpool.add_thread(new lthread_test(2));
  // sys->lthreadpool.add_thread(new lthread_test(3));

  // sys->fthreadpool.add_thread(new fthread_test(0));
  // sys->fthreadpool.add_thread(new fthread_test(1));
  // sys->fthreadpool.add_thread(new fthread_test(2));
  // sys->fthreadpool.add_thread(new fthread_test(3));
  sys->fthreadpool.add_thread(new txrxwk(sys)    );
  sys->fthreadpool.add_thread(new pcap(sys)    );
}


