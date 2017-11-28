
export SSN_SDK = ../../../../
include $(SSN_SDK)/mk/ssn.vars.mk

CXXFLAGS += $(SSN_CXXFLAGS)
CXXFLAGS += -Wno-format-security -I. -g -O0 -std=c++11
CXXFLAGS += -fsanitize=address
LDFLAGS  += $(SSN_LDFLAGS)

SRC = main.cc
OBJ = $(SRC:.cc=.o)
TARGET = a.out

$(TARGET): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)
	@echo LD a.out

clean:
	rm -rf *.out *.o

run:
	sudo ./a.out \
		--vdev=net_tap0,iface=tap0 \
		--vdev=net_tap1,iface=tap1


