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
        TurboPForV 'chunk_series::Algo::TurboPForV' = 1

    cdef cppclass ChunkedWriter:
        ChunkedWriter(const int& _algo_num)
        void open(string path, size_t size)
        size_t write(np.npy_uint64* data, int size, int chunk_size)

    cdef cppclass ChunkedReader:
        ChunkedReader()
        void open(string path)
        size_t read_into_buffer(np.npy_uint64* data, int size)
        size_t size()


ALGO_STR = {
    'TurboPFor': Algo.TurboPFor,
    'TurboPForV': Algo.TurboPForV,
}


def algo_from_str(algo_str):
    return ALGO_STR[algo_str]


cdef class Writer:
    cdef ChunkedWriter *thisptr
    cdef string file_path

    def __cinit__(self, algo, str file_path):
        algo = algo_from_str(algo)
        self.thisptr = new ChunkedWriter(algo)
        self.file_path = file_path.encode()

    def __dealloc___(self):
        self.close()

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        self.close()

    def close(self):
        del self.thisptr

    def write(self, np.npy_uint64[::1] mview, chunk_size=100):
        self.thisptr.open(self.file_path, mview.shape[0]*8 + 1024)
        self.thisptr.write(&mview[0], mview.shape[0], chunk_size)


cdef class Reader:
    cdef ChunkedReader *thisptr

    def __cinit__(self, str file_path):
        self.thisptr = new ChunkedReader()
        self.thisptr.open(file_path.encode())

    def __dealloc___(self):
        self.close()

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        self.close()

    def close(self):
        del self.thisptr

    def read(self):
        size = self.thisptr.size()
        cdef np.ndarray[np.npy_uint64, ndim=1] result = np.zeros(size, dtype = np.uint64)

        self.thisptr.read_into_buffer(&result[0], size)

        return result

