
export SSN_SDK = $(HOME)/git/susanow
include $(SSN_SDK)/mk/ssn.vars.mk
CXXFLAGS += $(SSN_CXXFLAGS)
LDFLAGS  += $(SSN_LDFLAGS)

CXXFLAGS += -Wno-format-security -I. -g -O0 -std=c++11
CXXFLAGS += -fsanitize=address


all:
	@$(CXX) $(CXXFLAGS) main.cc $(LDFLAGS)
	@echo LD a.out

clean:
	rm -rf *.out *.o

runnull:
	sudo ./a.out \
		--vdev=eth_null0 \
		--vdev=eth_null1

runpcap:
	sudo ./a.out \
		--vdev=eth_pcap0,iface=tap0 \
		--vdev=eth_pcap1,iface=tap1

runtap:
	sudo ./a.out \
		--vdev=net_tap0,iface=tap0 \
		--vdev=net_tap1,iface=tap1


