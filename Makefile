

testtest:
	make -C test

re:
	make -C test re

run:
	sudo ./test/a.out \
		--vdev=eth_null0

	#--vdev=eth_null1
