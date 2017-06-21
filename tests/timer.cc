
#include <stdio.h>
#include <unistd.h>
#include <susanow.h>
#include <slankdev/extra/dpdk.h>

size_t num0=0;
size_t num1=1;
size_t num2=2;
size_t num3=3;
size_t num4=4;
size_t num5=5;

void Slankdev (void* arg)
{
  int* n = (int*)arg;
  printf("slankdev arg=%d lcore%u \n", *n, rte_lcore_id());
}

int main(int argc, char** argv)
{
  ssn_init(argc, argv);

  ssn_timer_sched_register(2);
  ssn_timer_sched_register(5);

  uint64_t hz = rte_get_timer_hz();
  ssn_timer* tim2 = ssn_timer_alloc(Slankdev , &num2, hz);
  ssn_timer* tim5 = ssn_timer_alloc(Slankdev , &num5, hz);
  ssn_timer_add(tim2, 2);
  ssn_timer_add(tim5, 5);

  sleep(2);
  sleep(1); ssn_timer_del(tim2, 2); ssn_timer_free(tim2);
  sleep(2); ssn_timer_del(tim5, 5); ssn_timer_free(tim5);
  ssn_timer_sched_unregister(2);
  ssn_timer_sched_unregister(5);

  ssn_fin();
}


