# Makefile to fool "karabo install" into building and installing the device
# using CMake.

CONF ?= Debug

package: install

# legacy target
build: install

install:
	@./.install.sh ${CONF} $(patsubst -j%,%,$(filter -j%,$(MAKEFLAGS)))

clean:
	rm -fr dist
	rm -fr build

test: install
	@cd build && \
	cmake .. -DBUILD_TESTS=1 && \
	cmake --build . && \
	cd imageSource && CTEST_OUTPUT_ON_FAILURE=1 ctest -VV
