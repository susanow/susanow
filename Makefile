

testtest:
	make -C test

re:
	make -C test re

run:
	sudo ./test/a.out \
		-b 00:01:00.0 -b 00:01:00.1 \
		--vdev=eth_null0 --vdev=eth_null1

