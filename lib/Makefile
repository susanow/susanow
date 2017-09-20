
all:
	make -C liblthread_dpdk
	make -C libvty
	make -C libsusanow

clean:
	make -C liblthread_dpdk clean
	make -C libvty          clean
	make -C libsusanow      clean

test:
	make -C libsusanow      test
