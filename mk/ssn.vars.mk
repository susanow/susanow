

DPDK_PATH        = $(SSN_SDK)/lib/dpdk/x86_64-native-linuxapp-gcc
LTHREAD_PATH     = $(SSN_SDK)/lib/liblthread_dpdk
LIBVTY_PATH      = $(SSN_SDK)/lib/libvty
LIBSLANKDEV_PATH = $(SSN_SDK)/lib/libslankdev
LIBDPDK_CPP_PATH = $(SSN_SDK)/lib/libdpdk_cpp
LIBSSN_PATH      = $(SSN_SDK)/lib/libsusanow
CROW_PATH        = $(SSN_SDK)/lib/crow

RTE_SDK = $(SSN_SDK)/lib/dpdk
RTE_TARGET = x86_64-native-linuxapp-gcc
include $(SSN_SDK)/lib/libdpdk_cpp/mk/dpdk.cpp.mk

SSN_LIB_CXXFLAGS += \
		-I$(DPDK_PATH)/include \
		-I$(LTHREAD_PATH)      \
		-I$(LIBVTY_PATH)       \
		-I$(LIBSLANKDEV_PATH)  \
		-I$(LIBDPDK_CPP_PATH)  \
		-I$(LIBSSN_PATH)       \
		-I$(CROW_PATH)/include \
		-m64 -mssse3           \
		-std=c++11             \
		-Wno-format-security

SSN_LIB_LDFLAGS  += \
	  -L$(DPDK_PATH)/lib         \
	  -L$(LTHREAD_PATH)          \
		-L$(LIBVTY_PATH)           \
		-L$(LIBSSN_PATH)           \
		-llthread_dpdk             \
		-lvty                      \
		-lpthread                  \
		-lboost_system             \
		-lboost_thread             \
		$(DPDK_LDFLAGS)

SSN_CFLAGS   =  $(DPDK_CXXFLAGS)
SSN_CXXFLAGS =  $(SSN_LIB_CXXFLAGS)
SSN_LDFLAGS  = -lsusanow $(SSN_LIB_LDFLAGS)


