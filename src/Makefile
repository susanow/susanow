

SSN := /home/slank/git/susanow
include $(SSN)/mk/vars.mk
CXXFLAGS += -Wno-format-security
CXXFLAGS += -g -fsanitize=address


TARGET = a.out
SRCS   = main.cc
OBJS   = $(SRCS:.cc=.o)
include $(SSN)/mk/rules.mk


all: $(TARGET)
clean:
	$(RM) $(TARGET) $(OBJS)

include ../mk/runenv.mk

