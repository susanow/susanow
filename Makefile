

def: build-nfvi

build-nfvi: test-lib
	make -C src

test-lib: build-lib
	make -C lib test

build-lib:
	make -C lib

clean:
	make -C lib clean
	make -C src clean
