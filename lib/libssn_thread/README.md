
# ssn\_lthread

lthreadのラッパー

```
#include <stdio.h>
#include <unistd.h>
#include "ssn_lthread.h"

void func(void*)
{
  for (size_t i=0; i<5; i++) {
    printf("tets\n");
    lthread_sleep(1000);
  }
}

int main(int argc, char** argv)
{
  slankdev::dpdk_boot(argc, argv);

  ssn_lthread_sched sched;
  sched.start_scheduler(1);
  sched.launch(func, nullptr);
  sleep(5);
  sched.stop_scheduler();

  rte_eal_mp_wait_lcore();
}
```

## libssnのリンクの仕方　

```
include $(SSN_SDK)/mk/lib.mk
CXXFLAGS += $(SSN_CXXFLAGS)
LDFLAGS  += $(SSN_LDFLAGS)

def:
	$(CXX) $(CXXFLAGS) main.cc $(LDFLAGS)
```
