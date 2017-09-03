#!/bin/bash

function install_packages() {
	git submodule init
	git submodule update
	sudo apt install -y libpcap-dev python linux-headers-`uname -r`
}

function build_dpdk() {
	echo "Build DPDK"
	cd lib/dpdk
	export RTE_SDK=`pwd`
	export RTE_TARGET=x86_64-native-linuxapp-gcc
	make install T=$RTE_TARGET
	if [ ! $? = "0" ]; then
		exit -1;
	fi
	cd ../../
}

function build_libs() {
	make -C lib
}

function test_libs() {
	make -C lib/libsusanow/tests all
	if [ ! $? = "0" ]; then
		exit -1
	fi
}


install_packages
build_dpdk
build_libs
test_libs
echo "Success Setup for SUSANOW"

