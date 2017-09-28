

# MIT License
#
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


include ../../mk/ssn.vars.mk
include ../../mk/rules.mk

CXXFLAGS = $(SSN_LIB_CXXFLAGS) -Wno-format-security
LDFLAGS  = $(SSN_LIB_LDFLAGS)

TARGET = libsusanow.a
SRC = \
			ssn_cpu.cc     \
			ssn_vty.cc     \
			ssn_config.cc  \
			ssn_port.cc    \
			ssn_timer.cc   \
			ssn_log.cc     \
			ssn_rest.cc    \
			ssn_port.cc    \
			ssn_port_stat.cc     \
			ssn_green_thread.cc  \
			ssn_native_thread.cc \
			ssn_thread.cc        \
			ssn_ring.cc          \
			ssn_common.cc        \
			ssn_ma_port.cc       \
			ssn_vnf.cc
OBJ = $(SRC:.cc=.o)

$(TARGET): $(OBJ)
	@ar rcs libsusanow.a $(OBJ)
	@echo AR libsusanow.a

doc:
	$(MAKE) -C docs

clean:
	rm -rf *.o *.out *.a
	make -C examples clean

test:
	make -C examples all
