SHELL := /bin/bash

CXX       = g++-10
CFLAGS    = -march=native -O3 -std=c++2a -Wall -Wextra -pedantic

TURBOPFOR ?= ./ext/TurboPFor-Integer-Compression
src        = $(wildcard src/*.cpp)


all:


clean:
	rm -rf build
	rm -rf $(TURBOPFOR)


build_deps: 
	#git clone git@github.com:powturbo/TurboPFor-Integer-Compression.git $(TURBOPFOR)
	CLAGS='-fPIC' make -C $(TURBOPFOR) -j


.PHONY: build
build: 
	mkdir -p build
	$(CXX) $(CFLAGS) -o build/numstore $(src) \
		 $(TURBOPFOR)/bitunpack.o \
		 $(TURBOPFOR)/bitunpack_sse.o \
		 $(TURBOPFOR)/bitunpack_avx2.o \
		 $(TURBOPFOR)/bitpack.o \
		 $(TURBOPFOR)/bitpack_sse.o \
		 $(TURBOPFOR)/bitpack_avx2.o \
		 $(TURBOPFOR)/vp4c.o \
		 $(TURBOPFOR)/vp4c_sse.o \
		 $(TURBOPFOR)/vp4c_avx2.o \
		 $(TURBOPFOR)/vp4d.o \
		 $(TURBOPFOR)/vp4d_sse.o \
		 $(TURBOPFOR)/vp4d_avx2.o \
		 $(TURBOPFOR)/vint.o \
		 $(TURBOPFOR)/vsimple.o \
		 $(TURBOPFOR)/bitutil.o


build_lib:
	$(CXX) -I./src/ $(CFLAGS) -fPIC src/mapped.cpp src/chunk_series.cpp \
         $(TURBOPFOR)/bitunpack.o \
         $(TURBOPFOR)/bitunpack_sse.o \
         $(TURBOPFOR)/bitunpack_avx2.o \
         $(TURBOPFOR)/bitpack.o \
         $(TURBOPFOR)/bitpack_sse.o \
         $(TURBOPFOR)/bitpack_avx2.o \
         $(TURBOPFOR)/vp4c.o \
         $(TURBOPFOR)/vp4c_sse.o \
         $(TURBOPFOR)/vp4c_avx2.o \
         $(TURBOPFOR)/vp4d.o \
         $(TURBOPFOR)/vp4d_sse.o \
         $(TURBOPFOR)/vp4d_avx2.o \
         $(TURBOPFOR)/vint.o \
         $(TURBOPFOR)/vsimple.o \
         $(TURBOPFOR)/bitutil.o \
	 -shared -o libnumstore.so

build_cython:
	source ./venv/bin/activate && \
	  export LDFLAGS="-L." && \
	  python3 setup.py build_ext -i
