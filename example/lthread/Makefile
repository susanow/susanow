

SSN := /home/slank/git/susanow
include $(SSN)/mk/vars.mk
CXXFLAGS += -Wno-format-security -g -fsanitize=address
CFLAGS += -I../../lthread -I../../lthread/arch/x86

CSRCS = \
	../../lthread/lthread.c       \
	../../lthread/lthread_sched.c \
	../../lthread/lthread_cond.c  \
	../../lthread/lthread_tls.c   \
	../../lthread/lthread_mutex.c \
	../../lthread/lthread_diag.c  \
	../../lthread/arch/x86/ctx.c
COBJS = $(CSRCS:.c=.o)
CXXSRCS = main.cc
CXXOBJS = $(CXXSRCS:.cc=.o)
TARGET = a.out

include $(SSN)/mk/rules.mk


clean:
	$(RM) $(TARGET) $(CXXOBJS) $(COBJS)

runcpu:
	sudo ./a.out --vdev=eth_null0 --vdev=eth_null1

include $(SSN)/mk/runenv.mk

