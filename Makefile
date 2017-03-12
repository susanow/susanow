

SSN := /home/slank/git/susanow
include $(SSN)/mk/vars.mk
CXXFLAGS += -Wno-format-security -g -fsanitize=address
CXXFLAGS += -I./slanklib
CXXFLAGS += -I./include


TARGET = a.out
SRCS   = main.cc
OBJS   = $(SRCS:.cc=.o)
include $(SSN)/mk/rules.mk


all: $(OBJS)
	@echo LD a.out
	@$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)


clean:
	$(RM) $(TARGET) $(OBJS)

runcpu:
	sudo ./a.out --vdev=eth_null0 --vdev=eth_null1


include $(SSN)/mk/runenv.mk

