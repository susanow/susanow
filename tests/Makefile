
export SSN_SDK = ../
include $(SSN_SDK)/mk/ssn.vars.mk
CXXFLAGS += $(SSN_CXXFLAGS)
LDFLAGS  += $(SSN_LDFLAGS)

CXXFLAGS += -Wno-format-security -I. -g -O0 -std=c++11
CXXFLAGS += -fsanitize=address

.cc.out:
	@echo "CXX $@"
	@$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)


SRC = \
			green_thread.cc  \
			native_thread.cc \
			rest.cc          \
			timer.cc         \
			port.cc          \
			port_stat.cc
OBJ = $(SRC:.cc=.out)

poc: rest.out

build: $(OBJ)
	@echo all-build done

test: build
	sudo ./timer.out
	sudo ./native_thread.out
	sudo ./green_thread.out
	sudo ./port.out
	sudo ./port_stat.out

clean:
	rm -rf *.out *.o

re: clean poc

