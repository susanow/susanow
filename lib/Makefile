

LIBSLANKDEV := $(HOME)/git/slankdev/libslankdev
CXXFLAGS += -I$(LIBSLANKDEV) -std=c++11 -Wno-format-security

# include $(HOME)/git/slankdev/libslankdev/mk/dpdk.mk
DPDK_CFLAGS = -I$(RTE_SDK)/$(RTE_TARGET)/include
DPDK_LDFLAGS = -L$(RTE_SDK)/$(RTE_TARGET)/lib -ldpdk -lpthread -ldl -lrt
CXXFLAGS += $(DPDK_CFLAGS) -I. -fsanitize=address -fsanitize=leak
LDFLAGS  += $(DPDK_LDFLAGS) -llthread

.cc.o:
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo CXX $@

SRC = ssn_sys.cc ssn_vty.cc ssn_config.cc ssn_port.cc
OBJ = $(SRC:.cc=.o)

def: $(OBJ)
	ar rcs libsusanow.a $(OBJ)

clean:
	rm -rf *.o *.out *.a

