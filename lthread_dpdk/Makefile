
include $(HOME)/git/slankdev/libslankdev/mk/dpdk.mk

CC = gcc
CFLAGS  = $(DPDK_CFLAGS) -Iarch/x86 -I.
LDFLAGS = $(DPDK_LDFLAGS)

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

all: $(OBJ)
	ar rcs liblthread_dpdk.a $(OBJ)

clean:
	rm -rf *.a *.o

