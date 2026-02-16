all: build/c_compiler

build/c_compiler:
	mkdir -p build
	cmake -S . -B build
	$(MAKE) -C build

clean:
	rm -rf build
