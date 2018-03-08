.PHONY: default install clean clean-all

default:
	make -C cpp-libs/core/src
	make -C src
	cp ./src/gateway-linux ./bin/
clean:
	make -C cpp-libs/core/src clean
	make -C src clean
	rm -f ./bin/gateway-linux ./src/version.h
