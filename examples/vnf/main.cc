

#include <stdio.h>
#include <ssn_log.h>
#include <ssn_port.h>
#include <ssn_sys.h>
#include "susanow.h"

extern bool run;
void print(void* arg)
{
  ssn* s = reinterpret_cast<ssn*>(arg);
  while (run) {
    s->debug_dump(stdout);
    printf("-------------\n");
    ssn_sleep(1000);
  }
}

int main(int argc, char** argv)
{
  ssn_log_set_level(SSN_LOG_EMERG);
  ssn_log_set_level(SSN_LOG_DEBUG);

  ssn s(argc, argv);
  ssn_native_thread_launch(print, &s, 7);

  while (true) {
    char c = getchar();
    if (c == 'q') {
      printf("quit\n");
      break;
    }
  }
  run = false;
  ssn_timer_sched_poll_thread_stop();
}

