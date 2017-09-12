

#include <stdio.h>
#include <ssn_cpu.h>
#include <ssn_common.h>
#include <ssn_log.h>
#include <ssn_thread.h>

size_t num0 = 0;
size_t num1 = 1;
size_t num2 = 2;
size_t num3 = 3;

void func(void* arg)
{
  size_t num = *reinterpret_cast<size_t*>(arg);
  for (size_t i=0; i<3; i++) {
    printf("%02lx: func arg=%zd \n", i, num);
    ssn_sleep(1000);
  }
}

int main(int argc, char** argv)
{
  /*-BOOT-BEGIN--------------------------------------------------------------*/
  ssn_log_set_level(SSN_LOG_DEBUG);
  ssn_init(argc, argv);
  ssn_green_thread_sched_register(1);
  /*-BOOT-END----------------------------------------------------------------*/

  ssn_thread_launch(func, &num1, 2);
  ssn_thread_wait(2);

  /*-FINI-BEGIN--------------------------------------------------------------*/
  ssn_green_thread_sched_unregister(1);
  ssn_wait_all_lcore();
  ssn_fin();
  /*-FINI-END----------------------------------------------------------------*/
}




