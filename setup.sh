#!/bin/sh
git submodule init
git submodule update

echo "Build lthread"
cd lthread
cmake .
make
cd ..

echo "Build DPDK"
cd dpdk
export RTE_SDK=`pwd`
export RTE_SDK=x86_64-native-linuxapp-gcc
make install T=$RTE_TARGET
cd ..

echo "Success Setup for SUSANOW"
