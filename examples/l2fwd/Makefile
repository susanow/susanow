
export SSN_SDK = $(HOME)/git/susanow
include $(SSN_SDK)/mk/ssn.vars.mk
CXXFLAGS += $(SSN_CXXFLAGS)
LDFLAGS  += $(SSN_LDFLAGS)

CXXFLAGS += -Wno-format-security -I. -g -O0 -std=c++11
CXXFLAGS += -fsanitize=address

.cc.out:
	@echo "CXX $@"
	@$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)


SRC = lthread.cc timer.cc port.cc main.cc
OBJ = $(SRC:.cc=.out)

poc: main.out

build: $(OBJ)
	@echo all-build done

test: build
	sudo ./timer.out
	sudo ./lthread.out

clean:
	rm -rf *.out *.o

re: clean poc

