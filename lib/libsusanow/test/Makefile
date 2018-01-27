
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
			test_ssn_thread.cc        \
			test_ssn_green_thread.cc  \
			test_ssn_native_thread.cc \
			test_ssn_vty.cc           \
			test_ssn_timer.cc         \
			test_ssn_port_stat.cc     \
			test_ssn_ma_port.cc       \
			test_ssn_ma_ring.cc
OBJ = $(SRC:.cc=.out)

# poc: clean
# 	$(CXX) $(CXXFLAGS) vty.cc $(LDFLAGS)

all: $(OBJ)
	@echo all-build done

clean:
	rm -rf *.out *.o

re: clean poc

