from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

from distutils import sysconfig
import numpy


setup(
    ext_modules=[
        Extension(
            "numstore",
            [
                "numstore.pyx",
            ],
            language="c++",
            include_dirs=[numpy.get_include()],
            libraries=["numstore"],
            extra_compile_args=["-I./include/", "-I./ext/", "-fPIC", "-std=c++2a"],
        )
    ],
    cmdclass={"build_ext": build_ext},
)
