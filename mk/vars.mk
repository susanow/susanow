

DPDK_DIR   := $(RTE_SDK)/$(RTE_TARGET)

INCLUDES += \
	 -I$(DPDK_DIR)/include \
	 -I$(SSN)/lib/include \
	 -include $(DPDK_DIR)/include/rte_config.h

LDFLAGS += \
	-Wl,--no-as-needed \
	-Wl,-export-dynamic \
	-L$(DPDK_DIR)/lib \
	-lpthread -ldl -lrt -lm -lpcap \
	-Wl,--whole-archive -Wl,--start-group \
	-ldpdk \
	-Wl,--end-group -Wl,--no-whole-archive

CXXFLAGS += -Wall -Wextra
ifeq ($(CXX), clang++)
CXXFLAGS += -Weverything
endif
CXXFLAGS += -std=c++11
CXXFLAGS += -m64 -pthread -march=native $(INCLUDES)
CXXFLAGS += -Wno-format-security


