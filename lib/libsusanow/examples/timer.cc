
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <ssn_timer.h>
#include <ssn_green_thread.h>
#include <ssn_native_thread.h>
#include <ssn_cpu.h>
#include <ssn_common.h>

size_t num0=0;
size_t num1=1;
size_t num2=2;
size_t num3=3;
size_t num4=4;
size_t num5=5;

void Slankdev (void* arg)
{
  int* n = (int*)arg;
  printf("slankdev arg=%d lcore%zd \n", *n, ssn_lcore_id());
}

int main(int argc, char** argv)
{
  ssn_init(argc, argv);

  ssn_timer_sched tm2(2);
  ssn_timer_sched tm3(3);
  ssn_native_thread_launch(ssn_timer_sched_poll_thread, &tm2, 2);
  ssn_native_thread_launch(ssn_timer_sched_poll_thread, &tm3, 3);

  uint64_t hz = ssn_timer_get_hz();
  ssn_timer* tim2 = new ssn_timer(Slankdev , &num2, hz);
  ssn_timer* tim3 = new ssn_timer(Slankdev , &num3, hz);
  tm2.add(tim2);
  tm3.add(tim3);

  sleep(2);
  sleep(1); tm2.del(tim2); delete (tim2);
  sleep(2); tm3.del(tim3); delete (tim3);

  ssn_fin();
}


