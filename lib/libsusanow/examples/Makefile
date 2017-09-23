
export SSN_SDK = ../../../
include $(SSN_SDK)/mk/ssn.vars.mk
CXXFLAGS += $(SSN_CXXFLAGS)
LDFLAGS  += $(SSN_LDFLAGS)

CXXFLAGS += -Wno-format-security -I. -g -O0 -std=c++11
CXXFLAGS += -fsanitize=address

.cc.out:
	@echo "CXX $@"
	@$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

SRC = \
			example_ssn_thread.cc        \
			example_ssn_green_thread.cc  \
			example_ssn_native_thread.cc \
			example_ssn_rest.cc          \
			example_ssn_vty.cc           \
			example_ssn_timer.cc         \
			example_ssn_port.cc          \
			example_ssn_port_stat.cc     \
			example_ssn_vnf_port.cc
OBJ = $(SRC:.cc=.out)

# poc: clean
# 	$(CXX) $(CXXFLAGS) vty.cc $(LDFLAGS)

all: $(OBJ)
	@echo all-build done

clean:
	rm -rf *.out *.o

re: clean poc

