
#include <stdio.h>
#include <unistd.h>
#include <susanow.h>
#include <ssn_lthread2.h>

size_t num0=0;
size_t num1=1;
size_t num2=2;
size_t num3=3;
size_t num4=4;
size_t num5=5;


void Slankdev(void* arg)
{
  size_t* n = (size_t*)arg;
  for (size_t i=0; i<15; i++) {
    printf("slankdev arg=%zd lcore=%u\n", n?*n:-1, rte_lcore_id());
    lthread_sleep(1000000000);
    lthread_yield();
  }
  printf("%s ret\n", __func__);
}
void Slankdev4(void* arg)
{
  size_t* n = (size_t*)arg;
  for (size_t i=0; i<4; i++) {
    printf("slankdev4 arg=%zd lcore=%u\n", n?*n:-1, rte_lcore_id());
    lthread_sleep(1000000000);
    lthread_yield();
  }
  printf("%s ret\n", __func__);
}

bool run = true;
void test(void*)
{
  sleep(2);
  while (run) {
    ssn_lthread_debug_dump(stdout, 4);
    sleep(1);
  }
}


int main(int argc, char** argv)
{
  ssn_init(argc, argv);
  ssn_lthread_init();
  ssn_launch(test, nullptr, 2);

  ssn_lthread_sched_register(4);
  ssn_lthread_launch(Slankdev4, &num2, 4);
  sleep(3);
  sleep(3);
  ssn_lthread_launch(Slankdev4, &num5, 4);

  sleep(6);
  // run = false;
  // ssn_lthread_sched_unregister(4);
  ssn_fin();
}


