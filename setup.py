from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

from distutils import sysconfig
import numpy

sysconfig.get_config_vars()['CXX'] = 'g++-10'

turbo_base="./ext/TurboPFor-Integer-Compression"
objs = [
    'bitpack.o',
    'bitpack_sse.o',
    'bitpack_avx2.o',
    'bitunpack.o',
    'bitunpack_sse.o',
    'bitunpack_avx2.o',
    'vp4c.o',
    'vp4c_avx2.o',
    'vp4c_sse.o',
    'vp4d.o',
    'vp4d_avx2.o',
    'vp4d_sse.o',
    'vint.o',
    'vsimple.o',
    'bitutil.o'
]
objs = [f'{turbo_base}/{o}' for o in objs]

setup(
        ext_modules=[
            Extension("numstore",
                [
                    "numstore.pyx",
                    #"src/chunk_series.cpp",
                    #"src/mapped.cpp",
                ],
                language="c++",
                include_dirs=[numpy.get_include()],
                #extra_objects=objs,
                libraries=['numstore'],
                extra_compile_args=['-I./src/', '-fPIC', "-std=c++2a"],
            )
        ],
        cmdclass = {'build_ext': build_ext}
)
