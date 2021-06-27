from libcpp.vector cimport vector
import cython

cdef extern from "src/reader.cpp":
    cdef cppclass ZReader:
        ZReader(int)
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
