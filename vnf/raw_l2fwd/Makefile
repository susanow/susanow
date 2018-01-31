
# MIT License
# Copyright (c) 2018 Susanow
# Copyright (c) 2018 Hiroki SHIROKURA
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.


include $(SSN_SDK)/mk/ssn.vars.mk
include $(SSN_SDK)/mk/ssn.rule.mk

CFLAGS   += $(SSN_CFLAGS)
CXXFLAGS += $(SSN_CXXFLAGS)
LDFLAGS  += $(SSN_LDFLAGS)

CXXSRC = main.cc
CXXOBJ = $(CXXSRC:.cc=.o)
OBJ    = $(CXXOBJ) $(SSN_SDK)/lib/dpdk_pmd/rte_eth_pipe.o
TARGET = a.out

TARGET: $(OBJ)
	@echo LD $(TARGET)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

clean:
	rm -rf *.out *.o $(TARGET)

run:
	sudo ./$(TARGET) -m 8000,0 --proc-type=secondary


