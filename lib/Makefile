

all:
	$(MAKE) -C liblthread_dpdk
	$(MAKE) -C libvty
	$(MAKE) -C libsusanow
	$(MAKE) -C dpdk_pmd

clean:
	$(MAKE) -C liblthread_dpdk clean
	$(MAKE) -C libvty          clean
	$(MAKE) -C libsusanow      clean
	$(MAKE) -C dpdk_pmd        clean

test:
	$(MAKE) -C libsusanow      test
