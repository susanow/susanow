
include $(SSN_SDK)/mk/ssn.vars.mk

CC = gcc
CFLAGS  = -Iarch/x86 -I. -I$(DPDK_PATH)/include -O3 -Ofast

TARGET = liblthread_dpdk.a
SRC = \
	arch/x86/ctx.c   \
	lthread.c        \
	lthread_cond.c   \
	lthread_diag.c   \
	lthread_mutex.c  \
	lthread_sched.c  \
	lthread_tls.c
OBJ = $(SRC:.c=.o)

.c.o:
	@echo "CC $@"
	@$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	@echo "AR $@"
	@ar rcs liblthread_dpdk.a $(OBJ)

clean:
	rm -rf *.a *.o

