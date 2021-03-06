
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

RTE_SDK := $(SSN_SDK)/lib/dpdk
RTE_TARGET := x86_64-native-linuxapp-gcc
INSTALLPATH_BIN  := /usr/local/bin
INSTALLPATH_SERV := /etc/systemd/system
LIB_DIR  := lib
NFVI_DIR := nfvi

.PHONY: test lib nfvi clean install uninstall setup

def: nfvi

nfvi: lib test
	make -C $(NFVI_DIR)

test: lib
	make -C $(LIB_DIR) test

lib:
	make -C $(LIB_DIR)

setup:
	git submodule init
	git submodule update
	sudo apt install -y        \
		libnuma-dev              \
		libpcap-dev python       \
		linux-headers-`uname -r` \
		build-essential          \
		libtcmalloc-minimal4     \
		libboost-system-dev      \
		libboost-thread-dev      \
		python3-pip
	sudo pip3 install requests
	make -C $(LIB_DIR)/dpdk install T=$(RTE_TARGET)

clean:
	make -C $(LIB_DIR)  clean
	make -C $(NFVI_DIR) clean

install:
	cp $(NFVI_DIR)/susanow $(INSTALLPATH_BIN)
	cp systemd/susanow.service $(INSTALLPATH_SERV)

uninstall:
	rm -f $(INSTALLPATH_BIN)/susanow
	rm -f $(INSTALLPATH_SERV)/susanow.service

