

# DPDK_PATH := ???

DPDK_CFLAGS = \
	 -I$(DPDK_PATH)/include \
	 -include $(DPDK_PATH)/include/rte_config.h

DPDK_LDFLAGS += \
	-Wl,--no-as-needed \
	-Wl,-export-dynamic \
	-L$(DPDK_PATH)/lib \
	-lpthread -ldl -lrt -lm -lpcap \
	-Wl,--whole-archive -Wl,--start-group \
	-ldpdk \
	-Wl,--end-group -Wl,--no-whole-archive
