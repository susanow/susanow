

include mk/dpdk.mk

CFLAGS  += $(DPDK_CFLAGS)
LDFLAGS += $(DPDK_LDFLAGS)


CFLAGS += -Wall -Wextra
ifeq ($(CXX), clang)
CFLAGS += -Weverything
endif
CFLAGS += -m64 -pthread -march=native
CFLAGS += -Wno-format-security

CXXFLAGS += $(CFLAGS)
CXXFLAGS += -std=c++11
