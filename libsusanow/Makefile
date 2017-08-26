

include ../mk/ssn.vars.mk
include ../mk/rules.mk

CXXFLAGS = $(SSN_LIB_CXXFLAGS) -Wno-format-security
LDFLAGS  = $(SSN_LIB_LDFLAGS)

TARGET = libsusanow.a
SRC = \
			ssn_cpu.cc     \
			ssn_vty.cc     \
			ssn_config.cc  \
			ssn_port.cc    \
			ssn_timer.cc   \
			ssn_log.cc     \
			ssn_rest.cc    \
			ssn_port.cc    \
			ssn_port_stat.cc     \
			ssn_green_thread.cc  \
			ssn_native_thread.cc \
			ssn_ring.cc          \
			ssn_common.cc
OBJ = $(SRC:.cc=.o)

$(TARGET): $(OBJ)
	@ar rcs libsusanow.a $(OBJ)
	@echo AR libsusanow.a

clean:
	rm -rf *.o *.out *.a

