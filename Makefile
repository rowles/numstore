SHELL := /bin/bash

CXX       ?= g++
CFLAGS    ?= -O3 -std=c++2a -Wall -Wextra -pedantic

TURBOPFOR ?= ./ext/TurboPFor-Integer-Compression
src        = $(wildcard src/*.cpp)


all:


clean:
	rm -rf build
	rm -rf $(TURBOPFOR)


build_deps: clean
	git clone git@github.com:powturbo/TurboPFor-Integer-Compression.git $(TURBOPFOR)
	make -C $(TURBOPFOR) -j


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
