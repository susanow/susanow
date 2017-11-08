
export SSN_SDK = $(HOME)/git/susanow
include $(SSN_SDK)/mk/ssn.vars.mk
CXXFLAGS += $(SSN_CXXFLAGS)
# CXXFLAGS += -I$(SSN_SDK)/lib/crow/include
# CXXFLAGS += -I$(SSN_SDK)/lib/crow/include/crow
CXXFLAGS += -Wno-format-security -I. -g -O0 -std=c++11

LDFLAGS  += $(SSN_LDFLAGS)
LDFLAGS  += -lpthread -lboost_system -lboost_thread


all:
	@$(CXX) $(CXXFLAGS) main.cc $(LDFLAGS)
	@echo LD a.out

clean:
	rm -rf *.out *.o

run:
	sudo ./a.out \
		--vdev=net_tap0,iface=tap0 \
		--vdev=net_tap1,iface=tap1


