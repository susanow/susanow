
#include <stdio.h>
#include <unistd.h>
#include <susanow.h>

size_t num0=0;
size_t num1=1;
size_t num2=2;
size_t num3=3;
size_t num4=4;
size_t num5=5;

void Slankdev (void* arg)
{
  int* n = (int*)arg;
  printf("slankdev %d lcore%u \n", *n, rte_lcore_id());
}
void Slankdev1(void*)
{ printf("slankdev1 lcore%u \n", rte_lcore_id()); }

int main(int argc, char** argv)
{
  ssn_init(argc, argv);
  ssn_tmsched_register(2);
  ssn_tmsched_register(5);

  uint64_t hz = rte_get_timer_hz();
  ssn_timer* tim1 = ssn_timer_alloc(Slankdev , &num1, hz, 2);
  ssn_timer* tim2 = ssn_timer_alloc(Slankdev , &num2, hz, 2);
  ssn_timer* tim3 = ssn_timer_alloc(Slankdev1, nullptr, hz, 5);
  ssn_timer_add(tim1);
  ssn_timer_add(tim2);
  ssn_timer_add(tim3);

  ssn_cpu_debug_dump(stdout);
  ssn_timer_debug_dump(stdout);

  sleep(3);
  sleep(1); ssn_timer_del(tim1); ssn_timer_free(tim1, 2);
  sleep(1); ssn_timer_del(tim2); ssn_timer_free(tim2, 2);
  sleep(4); ssn_timer_del(tim3); ssn_timer_free(tim3, 5);
  ssn_tmsched_unregister(2);
  ssn_tmsched_unregister(5);
  rte_eal_mp_wait_lcore();
}


