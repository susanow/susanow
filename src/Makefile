


TARGET = a.out
SRCS   = main.cc
OBJS   = $(SRCS:.cc=.o)
include ../mk/vars.mk
CXXFLAGS += -g -fsanitize=address


all: $(TARGET)

clean:
	$(RM) $(TARGET) $(OBJS)

rr:
	make -C lib clean
	make clean
	make -C lib
	make


include ../mk/runenv.mk

