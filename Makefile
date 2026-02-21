all: build/c_compiler

build/c_compiler: build/Makefile
	$(MAKE) -C build

build/Makefile: CMakeLists.txt
	mkdir -p build
	cmake -S . -B build

test: build/c_compiler
	ctest --test-dir build/

clean:
	rm -rf build

.PHONY: all test clean
