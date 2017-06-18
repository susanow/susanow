

include $(SSN_SDK)/mk/ssn.vars.mk
CXXFLAGS += $(SSN_CXXFLAGS)
LDFLAGS  += $(SSN_LDFLAGS)

CXXFLAGS += -Wno-format-security -I. -g -O0 -std=c++11
CXXFLAGS += -fsanitize=address

def:
	$(CXX) $(CXXFLAGS) main.cc $(LDFLAGS)

clean:
	rm -rf *.out *.o
