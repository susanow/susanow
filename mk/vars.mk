

DPDK_DIR   := $(RTE_SDK)/$(RTE_TARGET)

INCLUDES += \
	 -I$(DPDK_DIR)/include \
	 -include $(DPDK_DIR)/include/rte_config.h

LDFLAGS += \
	-Wl,--no-as-needed \
	-Wl,-export-dynamic \
	-L$(DPDK_DIR)/lib \
	-lpthread -ldl -lrt -lm -lpcap \
	-Wl,--whole-archive -Wl,--start-group \
	-ldpdk \
	-Wl,--end-group -Wl,--no-whole-archive



CFLAGS += -Wall -Wextra
ifeq ($(CXX), clang)
CFLAGS += -Weverything
endif
CFLAGS += -m64 -pthread -march=native $(INCLUDES)
CFLAGS += -Wno-format-security


CXXFLAGS += $(CFLAGS)
CXXFLAGS += -std=c++11
