# cython: language_level=3
# cython: boundscheck=False
# cython: infer_types=False

from libcpp.vector cimport vector
from libcpp.string cimport string
import cython

import numpy as np
cimport numpy as np


cdef extern from "chunk_series.h" namespace "chunk_series":
    enum Algo 'chunk_series::Algo':
        TurboPFor 'chunk_series::Algo::TurboPFor' = 0

    cdef cppclass ChunkedWriter:
        ChunkedWriter(const Algo& _algo)
        void open(string path, size_t size)
        size_t write(np.npy_uint64* data, int size)


cdef extern from "<valarray>" namespace "std":
    cdef cppclass valarray[T]:
        valarray()
        valarray(int)  # constructor: empty constructor
        T& operator[](int)  # get/set element

import numpy as np
cimport numpy as np

cdef class Writer:
    cdef ChunkedWriter *thisptr

    def __cinit__(self, file_path):
        self.thisptr = new ChunkedWriter(Algo.TurboPFor)

    def write(self, string file_path, np.npy_uint64[::1] mview):
        self.thisptr.open(file_path, 1000000)
        cdef size_t n = mview.shape[0]
        cdef np.npy_uint64* data = &mview[0]
        self.thisptr.write(&mview[0], len(mview))



"""
        int get_i()
        vector[int] get_v()
        void write(vector[unsigned int])
        #void write(unsigned int[::1])
        vector[unsigned int] read()
        #unsigned int[::1] read()

cdef class PyZReader:
    cdef ZReader *thisptr      # hold a C++ instance which we're wrapping
    def __cinit__(self, int i):
        self.thisptr = new ZReader(i)
    def __dealloc__(self):
        del self.thisptr
    def get_i(self):
        return self.thisptr.get_i()
    def get_v(self):
        return self.thisptr.get_v()
    @cython.boundscheck(False)
    @cython.wraparound(False)
    def write(self, vector[unsigned int] nums):
        return self.thisptr.write(nums)
    def read(self):
        return self.thisptr.read()
        """
