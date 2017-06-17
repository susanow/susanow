
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




include $(SSN_SDK)/mk/vars.mk

CXXFLAGS += -Wno-format-security -g
CXXFLAGS += -fsanitize=address
CXXFLAGS += -I./include -I./
CXXFLAGS += -I/home/slank/git/slankdev/libslankdev

LDFLAGS  += -llthread

TARGET = a.out
CXXSRCS   = \
	main.cc             \
	ssnlib_sys.cc       \
	ssnlib_port.cc      \
	ssnlib_thread.cc    \
	ssnlib_mempool.cc   \
	ssnlib_ring.cc      \
	ssnlib_port_impl.cc
CXXOBJS   = $(CXXSRCS:.cc=.o)
OBJS = $(CXXOBJS)

all: $(OBJS)
	@echo LD a.out
	@$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

clean:
	$(RM) $(TARGET) $(CXXOBJS)


include $(SSN_SDK)/mk/rules.mk
include $(SSN_SDK)/mk/depends.mk
include $(SSN_SDK)/mk/runenv.mk


