#!/bin/sh
git submodule init
git submodule update

sudo apt install -y libpcap-dev
sudo apt install -y linux-headers-`uname -r`

echo "Build DPDK"
cd dpdk
export RTE_SDK=`pwd`
export RTE_TARGET=x86_64-native-linuxapp-gcc
make install T=$RTE_TARGET
cd ..

make -C liblthread_dpdk
make -C libvty
make -C libsusanow
make -C tests build

echo "Success Setup for SUSANOW"
