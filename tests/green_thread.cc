
#include <stdio.h>
#include <unistd.h>
#include <susanow.h>

size_t num0=0;
size_t num1=1;
size_t num2=2;
size_t num3=3;
size_t num4=4;
size_t num5=5;

size_t nb_threads = 0;

void Slankdev(void* arg)
{
  nb_threads ++;
  size_t* n = (size_t*)arg;
  for (size_t i=0; i<5; i++) {
    printf("slankdev arg=%zd lcore=%zd\n", n?*n:-1, ssn_lcore_id());
    ssn_sleep(1000);
    ssn_yield();
  }
  nb_threads --;
}

int main(int argc, char** argv)
{
  ssn_init(argc, argv);

  ssn_green_thread_sched_register(1);
  ssn_green_thread_launch(Slankdev, &num2, 1);
  sleep(3);
  ssn_green_thread_launch(Slankdev, &num4, 1);

  while (nb_threads > 0) ;
  ssn_green_thread_sched_unregister(1);
  ssn_fin();
}


