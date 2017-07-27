
export SSN_SDK = $(HOME)/git/susanow
include $(SSN_SDK)/mk/ssn.vars.mk
CXXFLAGS += $(SSN_CXXFLAGS)
CXXFLAGS += -Wno-format-security -I. -std=c++11
CXXFLAGS += -fsanitize=address
CXXFLAGS += -g
CXXFLAGS += -O0
LDFLAGS  += $(SSN_LDFLAGS)


all:
	$(CXX) $(CXXFLAGS) main.cc vnf.cc stage.cc $(LDFLAGS)

clean:
	rm -rf *.out *.o

re: clean poc

