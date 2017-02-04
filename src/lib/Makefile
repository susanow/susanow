
AR := ar
RM := rm -rf

INCLUDES   += \
	 -I$(prefix)/src \
	 -I$(prefix)/src/include \
	 -I$(RTE_SDK)/$(RTE_TARGET)/include \
	 -I/home/slank/git/libslankdev/include \
	 -include $(RTE_SDK)/$(RTE_TARGET)/include/rte_config.h
INCLUDES += -I./include

CXXFLAGS += -Wall -Wextra
ifeq ($(CXX), clang++)
CXXFLAGS += -Weverything # Thankyou, @herumi.
endif
CXXFLAGS += -std=c++11
CXXFLAGS += -m64 -pthread -march=native $(INCLUDES)
CXXFLAGS += -Wno-format-security

LDFLAGS += \
	-Wl,--no-as-needed \
	-Wl,-export-dynamic \
	-L$(RTE_SDK)/$(RTE_TARGET)/lib \
	-lpthread -ldl -lrt -lm -lpcap \
	-Wl,--whole-archive \
	-Wl,--start-group \
	-ldpdk \
	-Wl,--end-group \
	-Wl,--no-whole-archive
LDFLAGS += -lreadline

SRCS   =
OBJS   = $(SRCS:.cc=.o)
TARGET = libsusanow.a

# .SUFFIXES: .out .c .cc .o .h
# .cc.o:
# 	@echo "CXX $@"
# 	@$(CXX) $(CXXFLAGS) -c $< -o $@
#
# $(TARGET): $(OBJS)
# 	@$(RM) -rf $@
# 	$(AR) rcs $@ $(OBJS)
#
# clean:
# 	$(RM) $(OBJS) $(TARGET)
#
# re: clean $(TARGET)
