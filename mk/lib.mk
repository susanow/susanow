
include $(SSN_SDK)/mk/dpdk.mk
SSN_CXXFLAGS += $(DPDK_CFLAGS) -std=c++11
SSN_LDFLAGS  += $(DPDK_LDFLAGS) -llthread

SLANKLLIB_PATH = /home/slank/git/slankdev/libslankdev
SSN_CXXFLAGS += -I$(SLANKLLIB_PATH)

SSNLIB_PATH = $(SSN_SDK)/lib/libssn_thread
SSN_CXXFLAGS += -I$(SSNLIB_PATH)
SSN_LDFLAGS  += -L$(SSNLIB_PATH) -lssn_thread

