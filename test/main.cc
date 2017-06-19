
#include <stdio.h>
#include <unistd.h>
#include <ssn_sys.h>
#include <ssn_timer.h>

/*-----------------------------------------------------------*/
size_t one=1;
size_t two=2;
size_t three=3;
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
/*-----------------------------------------------------------*/


int main(int argc, char** argv)
{
  ssn_init(argc, argv);
#if 0
  ssn_ltsched_register(1);
  ssn_launch(ssn_vty_thread    , nullptr, 1);
  ssn_launch(ssn_waiter_thread , nullptr, 1);
#endif
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
  return 0;

  sleep(3);
  sleep(1); ssn_timer_del(tim1); ssn_timer_free(tim1);
  sleep(1); ssn_timer_del(tim2); ssn_timer_free(tim2);
  sleep(4); ssn_timer_del(tim3);
  ssn_tmsched_unregister(2);
  ssn_tmsched_unregister(5);
  rte_eal_mp_wait_lcore();
}


