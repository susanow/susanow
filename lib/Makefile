

include ../mk/ssn.vars.mk
include ../mk/rules.mk

CXXFLAGS += $(SSN_CXXFLAGS) -Wno-format-security
LDFLAGS  += $(SSN_LDFLAGS)

SRC = ssn_sys.cc ssn_vty.cc ssn_config.cc ssn_port.cc
OBJ = $(SRC:.cc=.o)

def: $(OBJ)
	ar rcs libsusanow.a $(OBJ)

clean:
	rm -rf *.o *.out *.a

