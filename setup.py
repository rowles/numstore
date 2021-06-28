from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext


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
                    "src/reader.cpp",
                    "src/mapped.cpp",
                    "src/series.cpp",
                ], language="c++",
                extra_objects=objs,
                extra_compile_args=['-fPIC', "-std=c++2a"],
            )
        ],
        cmdclass = {'build_ext': build_ext}
)