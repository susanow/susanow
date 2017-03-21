

SSN := /home/slank/git/susanow
include $(SSN)/mk/vars.mk

LTHREAD_PATH := /home/slank/git/lthread
include $(LTHREAD_PATH)/lthread.mk

CXXFLAGS += -Wno-format-security -g
CXXFLAGS += -fsanitize=address
CXXFLAGS += -I./include
CXXFLAGS += -I/home/slank/git/libslankdev
CXXFLAGS += $(LTHREAD_CFLAGS)

TARGET = a.out
CXXSRCS   = \
	main.cc             \
	ssnlib_sys.cc       \
	ssnlib_port.cc      \
	ssnlib_thread.cc    \
	ssnlib_mempool.cc   \
	ssnlib_ring.cc      \
	ssnlib_port_impl.cc
CXXOBJS   = $(CXXSRCS:.cc=.o)
OBJS = $(CXXOBJS) $(LTHREAD_OBJS)


include $(SSN)/mk/rules.mk


all: $(OBJS)
	@echo LD a.out
	@$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

clean:
	$(RM) $(TARGET) $(CXXOBJS)


include $(SSN)/mk/runenv.mk

