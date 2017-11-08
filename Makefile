
SSN_SDK := `pwd`

all:
	@echo "SSN_SDK=$(SSN_SDK)"

build-lib:
	make -C lib

test-lib:
	make -C lib test
