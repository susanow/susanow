

#include <stdio.h>
#include <ssn_cpu.h>
#include <ssn_log.h>
#include <ssn_common.h>
#include <ssn_thread.h>

size_t num0 = 0;
size_t num1 = 1;
size_t num2 = 2;

void func(void* arg)
{
  auto lcore_id = ssn_lcore_id();
  size_t num = *reinterpret_cast<size_t*>(arg);
  for (size_t i=0; i<3; i++) {
    printf("%02lx: func arg=%zd lcore%zd\n", i, num, lcore_id);
    ssn_sleep(1000);
  }
}

int main(int argc, char** argv)
{
  ssn_log_set_level(SSN_LOG_EMERG);
  ssn_init(argc, argv);
  ssn_green_thread_sched_register(1);

  auto tid = ssn_thread_launch(func, &num1, 1);
  ssn_thread_join(tid);

  ssn_green_thread_sched_unregister(1);
  ssn_wait_all_lcore();
  ssn_fin();
}




