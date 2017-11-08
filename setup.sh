#!/bin/bash

function install_packages() {
	git submodule init
	git submodule update

	# Basic Dependencies
	sudo apt install -y        \
		libpcap-dev python       \
		linux-headers-`uname -r` \
		build-essential

	# ssnctl Dependencies
	sudo pip3 install requests

	# ipkn/crow Dependencies
	sudo apt install -y       \
		libtcmalloc-minimal4
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

install_packages
build_dpdk
echo "Success Setup for SUSANOW"

