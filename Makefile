SHELL := /bin/bash

CXX       = g++-10
CFLAGS    ?= -march=native -O3 -std=c++2a -Wall -Wextra -pedantic -fPIC

TURBOPFOR_REPO ?= https://github.com/rowles/TurboPFor-Integer-Compression.git
TURBOPFOR      ?= ./ext/TurboPFor-Integer-Compression
TURBOPFOR_OBJS = $(TURBOPFOR)/bitunpack.o \
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
INCLUDES = -I./ext/ -I./include/
SRC = src/mapped.cpp src/chunk_series.cpp
all:


clean:
	rm -rf *.so
	rm -rf $(TURBOPFOR)
	rm -rf venv
	rm -rf data/
	rm -rf build/
	rm -rf __pycache__/
	rm -rf numstore.cpp

setup_venv:
	python3 -m venv venv \
          && source ./venv/bin/activate \
          && pip install cython pytest numpy

build_ext:
	git clone $(TURBOPFOR_REPO) $(TURBOPFOR)
	make -C $(TURBOPFOR) -j

build_libnumstore:
	$(CXX) $(INCLUDES) $(CFLAGS) $(SRC) \
         $(TURBOPFOR_OBJS) -shared -o libnumstore.so

build_cython:
	source ./venv/bin/activate && \
	  export CC=$(CXX) && \
	  export LDFLAGS="-L." && \
	  python3 setup.py build_ext -i

test:
	source ./venv/bin/activate && \
	  export LD_LIBRARY_PATH="." && \
	  mkdir -p data/ && \
	  pytest -vvv test.py

benchmark:
	source ./venv/bin/activate && \
	  pip install pandas zarr pyarrow && \
	  export LD_LIBRARY_PATH="." && \
	  python benchmark.py

