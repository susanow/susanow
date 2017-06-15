

LIBSLANKDEV := /home/slank/git/slankdev/libslankdev
CXXFLAGS += -I$(LIBSLANKDEV) -std=c++11 -Wno-format-security

include /home/slank/git/slankdev/libslankdev/mk/dpdk.mk
CXXFLAGS += $(DPDK_CFLAGS) -I. -fsanitize=address -fsanitize=leak
LDFLAGS  += $(DPDK_LDFLAGS) -llthread

.cc.o:
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo CXX $@

SRC = ssn_sys.cc ssn_vty.cc
OBJ = $(SRC:.cc=.o)

def: $(OBJ)
	ar rcs libsusanow.a $(OBJ)

clean:
	rm -rf *.o *.out *.a

