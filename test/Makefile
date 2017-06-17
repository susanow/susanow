
SLANKLIB =  $(HOME)/git/slankdev/libslankdev

include $(SSN_SDK)/mk/ssn.vars.mk
CXXFLAGS += $(SSN_CXXFLAGS)
LDFLAGS  += $(SSN_LDFLAGS)

CXXFLAGS +=  -Wno-format-security -I. -g -O0 -I$(SLANKLIB)
CXXFLAGS += -fsanitize=address

def:
	$(CXX) $(CXXFLAGS) main.cc $(LDFLAGS)

clean:
	rm -rf *.out *.o
