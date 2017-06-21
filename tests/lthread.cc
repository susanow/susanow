
#include <stdio.h>
#include <unistd.h>
#include <susanow.h>

size_t num0=0;
size_t num1=1;
size_t num2=2;
size_t num3=3;
size_t num4=4;
size_t num5=5;

inline void _sleep() { printf("."); fflush(stdout); sleep(1); }

void test(void* arg)
{
  size_t* n = (size_t*)arg;
  printf("test: start\n");
  for (size_t i=0; i<3; i++) {
    printf("test arg=%zd \n", *n);
    ssn_sleep(1000);
  }
  printf("test: fin\n");
}

int main(int argc, char** argv)
{
  ssn_init(argc, argv);

  ssn_lthread_sched_register(1);
  // ssn_lthread_launch(test, &num0, 1);
  // ssn_lthread_launch(test, &num1, 1);
  _sleep();
  ssn_lthread_sched_unregister(1);

  ssn_fin();
}



