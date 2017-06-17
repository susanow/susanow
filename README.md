
# Susanow

A high Performance Networking Platform on DPDK.
Please see https://susanow.github.io


## Usage

install Required libs

```
$ git clone https://github.com/slankdev/libslankdev/git
$ cd libslankdev
$ sudo make install
$ cd ..

$ git clone https://github.com/susanow/lthread/git
$ cd lthread
$ make
$ cd ..
```

install DPDK

```
$ git clone http://github.com/susanow/dpdk.git $RTE_SDK
$ cd $RTE_SDK
$ make install T=$RTE_TARGET
```

clone Susanow and Build it

```
$ git clone http://github.com/susanow/susanow.git $SSN_SDK
$ cd $SSN_SDK
$ make
```


## Required Software

 - dpdk-16.11 (susanow/dpdk.git)
 - L-thread slankdev (susanow/lthread.git)
 - Libslankdev (slankdev/libslankdev.git)



## Licence and Author

MIT Licence, see LICENCE file

Hiroki SHIROKURA

 - E-mail slank.dev@gmail.com
 - Twitter @slankdev
 - Github  slankdev
 - Facebook hiroki.shirokura


