
# Susanow

Thread based high Performance Networking Platform on DPDK.
Please see https://susanow.github.io


## Usage

```
$ export SSN_SDK=/path/to/susanow_src
$ git clone http://github.com/susanow/susanow.git $SSN_SDK
$ cd $SSN_SDK
$ ./setup.sh
$ make
```

## Depend Software

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


## Memo

```
# for Lthread
LTHREAD_PATH = ./lthread
-I$(LTHREAD_PATH)/src
-L$(LTHREAD_PATH) -llthread

# for DPDK
-I$(RTE_SDK)/$(RTE_TARGET)/include
-L$(RTE_SDK)/$(RTE_TARGET)/lib

# for libslankdev
LIBSLANKDEV_PATH = ./libslankdev
-I$(LIBSLANKDEV_PATH)
```


