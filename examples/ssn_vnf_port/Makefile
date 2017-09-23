
include $(SSN_SDK)/mk/ssn.vars.mk
CXXFLAGS += $(SSN_CXXFLAGS) -I.
LDFLAGS  += $(SSN_LDFLAGS)

CXXFLAGS += -Wno-format-security -I. -g -O0 -std=c++11
CXXFLAGS += -fsanitize=address

SRC = main.cc ssn_vnf_port.cc

all:
	@$(CXX) $(CXXFLAGS) $(SRC) $(LDFLAGS)
	@echo LD a.out

clean:
	rm -rf *.out *.o

run:
	sudo ./a.out \
		--vdev=net_tap0,iface=tap0 \
		--vdev=net_tap1,iface=tap1


