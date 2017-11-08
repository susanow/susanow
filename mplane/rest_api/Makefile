

CXXFLAGS += -std=c++11
CXXFLAGS += -Wno-format-security
CXXFLAGS += -I $(HOME)/git/libslankdev
LDFLAGS  += -lboost_system -lboost_thread -lpthread

## DPDK
# LIBDPDK  = $(HOME)/git/libdpdk_cpp
# include $(LIBDPDK)/mk/dpdk.cpp.mk
# CXXFLAGS += -I$(LIBDPDK) $(DPDK_CXXFLAGS)
# LDFLAGS  += $(DPDK_LDFLAGS)

all:
	$(CXX) $(CXXFLAGS) main.cc $(LDFLAGS)

