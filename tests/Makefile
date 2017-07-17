
export SSN_SDK = ../
include $(SSN_SDK)/mk/ssn.vars.mk
CXXFLAGS += $(SSN_CXXFLAGS)
LDFLAGS  += $(SSN_LDFLAGS)

CXXFLAGS += -Wno-format-security -I. -g -O0 -std=c++11
CXXFLAGS += -fsanitize=address

.cc.out:
	@echo "CXX $@"
	@$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

SRC = \
			green_thread.cc  \
			native_thread.cc \
			rest.cc          \
			vty.cc           \
			timer.cc         \
			port.cc          \
			port_stat.cc     \
			vnf.cc
OBJ = $(SRC:.cc=.out)

poc: clean
	$(CXX) $(CXXFLAGS) vnf.cc ssn_vnf.cc $(LDFLAGS)

build: $(OBJ)
	@echo all-build done

clean:
	rm -rf *.out *.o

re: clean poc

