
export SSN_SDK = $(HOME)/git/susanow
include $(SSN_SDK)/mk/ssn.vars.mk
CXXFLAGS += $(SSN_CXXFLAGS)
CXXFLAGS += -Wno-format-security -I. -g -O0 -std=c++11
CXXFLAGS += -fsanitize=address
LDFLAGS  += $(SSN_LDFLAGS)


all:
	$(CXX) $(CXXFLAGS) main.cc susanow.cc $(LDFLAGS)

clean:
	rm -rf *.out *.o

re: clean poc

