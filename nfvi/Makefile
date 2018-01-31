
# MIT License
# Copyright (c) 2017 Susanow
# Copyright (c) 2017 Hiroki SHIROKURA
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

CXXFLAGS += -I. -std=c++11
CXXFLAGS += $(SSN_CXXFLAGS)
CFLAGS   += $(SSN_CFLAGS)
LDFLAGS  += $(SSN_LDFLAGS)

CXXSRC = \
	ssn_nfvi.cc         \
	ssn_json.cc         \
	ssn_rest_api.cc     \
	ssn_vnf.cc          \
	ssn_vnf_port.cc     \
	ports/virt.cc       \
	vnfs/l2fwd_delay.cc \
	vnfs/acl.cc         \
	main.cc
CSRC = rte_eth_pipe.c
OBJ  = $(CXXSRC:.cc=.o) $(CSRC:.c=.o)
TARGET = susanow

TARGET: $(OBJ)
	@echo LD $(TARGET)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

clean:
	rm -rf *.out *.o vnfs/*.o ports/*.o $(TARGET)

pch:
	$(CXX) $(CXXFLAGS) pch.h

cleanall: clean
	rm -rf *.pch
