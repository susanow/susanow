
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

INSTALLPATH_BIN  := /usr/local/bin
INSTALLPATH_SERV := /etc/systemd/system

.PHONY: test lib nfvi clean install uninstall setup

def: nfvi

nfvi: lib test
	make -C ssnnfvi

test: lib
	make -C lib test

lib:
	make -C lib

# cd lib/dpdk
setup:
	git submodule init
	git submodule update
	sudo apt install -y        \
		libpcap-dev python       \
		linux-headers-`uname -r` \
		build-essential          \
		libtcmalloc-minimal4     \
		libboost-system-dev      \
		libboost-thread-dev
	sudo pip3 install requests
	export RTE_SDK=`pwd`
	export RTE_TARGET=x86_64-native-linuxapp-gcc
	make -C lib/dpdk install T=$(RTE_TARGET)

clean:
	make -C lib clean
	make -C ssnnfvi clean

install:
	cp ssnnfvi/susanow $(INSTALLPATH_BIN)
	cp misc/susanow.service $(INSTALLPATH_SERV)

uninstall:
	rm -f $(INSTALLPATH_BIN)/susanow
	rm -f $(INSTALLPATH_SERV)/susanow.service

