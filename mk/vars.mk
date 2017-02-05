

DPDK_DIR   := $(RTE_SDK)/$(RTE_TARGET)
SSNLIB_DIR := /home/slank/git/susanow/src/lib

INCLUDES   += \
			 -I$(DPDK_DIR)/include \
			 -I$(SSNLIB_DIR)/include \
			 -include $(DPDK_DIR)/include/rte_config.h

LDFLAGS += \
	-Wl,--no-as-needed \
	-Wl,-export-dynamic \
	-L$(DPDK_DIR)/lib \
	-lpthread -ldl -lrt -lm -lpcap \
	-Wl,--whole-archive \
	-Wl,--start-group \
	-ldpdk \
	-Wl,--end-group \
	-Wl,--no-whole-archive
LDFLAGS += -lreadline

CXXFLAGS += -Wall -Wextra
ifeq ($(CXX), clang++)
CXXFLAGS += -Weverything # Thankyou, @herumi.
endif
CXXFLAGS += -std=c++11
CXXFLAGS += -m64 -pthread -march=native $(INCLUDES)
CXXFLAGS += -Wno-format-security



.SUFFIXES: .out .c .cc .o .h
.cc.o:
	@echo "CXX $@"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	@echo "LD $@"
	@$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)


