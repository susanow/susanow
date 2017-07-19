

#include <stdio.h>
#include <ssn_log.h>
#include <ssn_port.h>
#include <ssn_sys.h>
#include "susanow.h"

void print(void* arg)
{
  ssn* s = reinterpret_cast<ssn*>(arg);
  while (true) {
    s->debug_dump(stdout);
    printf("-------------\n");
    ssn_sleep(1000);
    ssn_yield();
  }
}

int main(int argc, char** argv)
{
  ssn_log_set_level(SSN_LOG_EMERG);
  ssn_log_set_level(SSN_LOG_DEBUG);

  ssn s(argc, argv);
  s.launch_green_thread(print, &s);

  while (true) {
    char c = getchar();
    if (c == 'q') {
      printf("quit\n");
      break;
    }
    s.vnf1->stages[1]->inc();
  }
  ssn_wait_all_lcore();
}

