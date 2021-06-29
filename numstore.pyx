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

    cdef cppclass ChunkedReader:
        ChunkedReader()
        void open(string path)
        size_t read_into_buffer(np.npy_uint64* data, int size)
        size_t size()


cdef class Writer:
    cdef ChunkedWriter *thisptr

    def __cinit__(self, file_path):
        self.thisptr = new ChunkedWriter(Algo.TurboPFor)

    def write(self, string file_path, np.npy_uint64[::1] mview):
        self.thisptr.open(file_path, mview.shape[0]*8 + 1024)
        self.thisptr.write(&mview[0], mview.shape[0])


cdef class Reader:
    cdef ChunkedReader *thisptr

    def __cinit__(self, file_path):
        self.thisptr = new ChunkedReader()
        self.thisptr.open(file_path)

    def read(self):
        size = self.thisptr.size()
        cdef np.ndarray[np.npy_uint64, ndim=1] result = np.zeros(size, dtype = np.uint64)

        self.thisptr.read_into_buffer(&result[0], size)

        return result

