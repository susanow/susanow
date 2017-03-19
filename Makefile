

SSN := /home/slank/git/susanow
include $(SSN)/mk/vars.mk
CXXFLAGS += -Wno-format-security -g
CXXFLAGS += -fsanitize=address
CXXFLAGS += -I./include
CXXFLAGS += -I/home/slank/git/libslankdev

CFLAGS += -I./lthread -I./lthread/arch/x86


CSRCS = \
	lthread/lthread.c       \
	lthread/lthread_sched.c \
	lthread/lthread_cond.c  \
	lthread/lthread_tls.c   \
	lthread/lthread_mutex.c \
	lthread/lthread_diag.c  \
	lthread/arch/x86/ctx.c
COBJS = $(CSRCS:.c=.o)
CXXSRCS   = \
	main.cc             \
	ssnlib_sys.cc       \
	ssnlib_port.cc      \
	ssnlib_thread.cc    \
	ssnlib_mempool.cc   \
	ssnlib_ring.cc      \
	ssnlib_port_impl.cc
CXXOBJS   = $(CXXSRCS:.cc=.o)
OBJS = $(CXXOBJS) $(COBJS)
TARGET = a.out

include $(SSN)/mk/rules.mk


all: $(OBJS)
	@echo LD a.out
	@$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)


clean:
	$(RM) $(TARGET) $(OBJS)


include $(SSN)/mk/runenv.mk

