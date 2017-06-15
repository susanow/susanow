

LIBSLANKDEV := /home/slank/git/slankdev/libslankdev
CXXFLAGS += -I$(LIBSLANKDEV) -std=c++11 -Wno-format-security

include /home/slank/git/slankdev/libslankdev/mk/dpdk.mk
CXXFLAGS += $(DPDK_CFLAGS)
LDFLAGS  += $(DPDK_LDFLAGS) -llthread

.cc.o:
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo CXX $@

SRC = ssn_lthread.cc
OBJ = $(SRC:.cc=.o)

def: $(OBJ)
	ar rcs libssn_thread.a $(OBJ)

clean:
	rm -rf *.o *.out

efdf:
	# @$(CXX) $(CXXFLAGS) $(OBJ) $(LDFLAGS)
	# @echo LD $@


