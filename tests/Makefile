
include $(SSN_SDK)/mk/ssn.vars.mk
CXXFLAGS += $(SSN_CXXFLAGS)
LDFLAGS  += $(SSN_LDFLAGS)

CXXFLAGS += -Wno-format-security -I. -g -O0 -std=c++11
CXXFLAGS += -fsanitize=address

def:
	$(CXX) $(CXXFLAGS) timer.cc   -o timer.out   $(LDFLAGS)
	$(CXX) $(CXXFLAGS) lthread.cc -o lthread.out $(LDFLAGS)

clean:
	rm -rf *.out *.o
