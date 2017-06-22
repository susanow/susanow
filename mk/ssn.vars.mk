

DPDK_PATH        = $(SSN_SDK)/dpdk/x86_64-native-linuxapp-gcc
LTHREAD_PATH     = $(SSN_SDK)/lthread_dpdk
LIBSLANKDEV_PATH = $(SSN_SDK)/libslankdev
LIBSSN_PATH      = $(SSN_SDK)/lib

SSN_LIB_CXXFLAGS += \
		-I$(DPDK_PATH)/include \
		-I$(LTHREAD_PATH)      \
		-I$(LIBSLANKDEV_PATH)  \
		-I$(LIBSSN_PATH)       \
		-m64 -mssse3           \
		-std=c++11

SSN_LIB_LDFLAGS  += \
	  -L$(DPDK_PATH)/lib         \
	  -L$(LTHREAD_PATH)          \
	  -L$(LIBSLANKDEV_PATH)      \
		-L$(LIBSSN_PATH)           \
		-llthread_dpdk \
		-ldpdk -lpthread -ldl -lrt

SSN_CXXFLAGS =  $(SSN_LIB_CXXFLAGS)
SSN_LDFLAGS  = -lsusanow $(SSN_LIB_LDFLAGS)


