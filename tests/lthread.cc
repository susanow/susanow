
#include <stdio.h>
#include <unistd.h>
#include <susanow.h>


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
    printf("slankdev arg=%zd lcore=%zd\n", n?*n:-1, ssn_lcore_id());
    ssn_sleep(1000);
    ssn_yield();
  }
  printf("%s ret\n", __func__);
}
void Slankdev4(void* arg)
{
  size_t* n = (size_t*)arg;
  for (size_t i=0; i<4; i++) {
    printf("slankdev4 arg=%zd lcore=%zd\n", n?*n:-1, ssn_lcore_id());
    ssn_sleep(1000);
    ssn_yield();
  }
  printf("%s ret\n", __func__);
}

int main(int argc, char** argv)
{
  ssn_init(argc, argv);
  ssn_lthread_init();

  ssn_lthread_sched_register(4);
  ssn_lthread_launch(Slankdev4, &num2, 4);
  ssn_lthread_launch(Slankdev4, &num5, 4);
  ssn_fin();
}


