

all:
	$(MAKE) -C liblthread_dpdk
	$(MAKE) -C libvty
	$(MAKE) -C libsusanow

clean:
	$(MAKE) -C liblthread_dpdk clean
	$(MAKE) -C libvty          clean
	$(MAKE) -C libsusanow      clean

test:
	$(MAKE) -C libsusanow      test
