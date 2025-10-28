import os
import glob
from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension, build_ext

SRC_DIR = "backend/src/"
DIST_DIR = "backend"
all_sources = glob.glob(os.path.join(SRC_DIR, "*.cpp"))
ext_modules = [
    Pybind11Extension(
        name=f"{DIST_DIR}.libcore",
        sources=all_sources,
        include_dirs=[os.path.join(os.getcwd(), SRC_DIR)],
        extra_compile_args=["-O3", "-std=c++17"],
    )
]
setup(
    name="libcore",
    version="0.1.0",
    description="FastAPI Backend with C++ Core (Pybind11)",
    cmdclass={"build_ext": build_ext},
    ext_modules=ext_modules,
    setup_requires=["pybind11>=2.10.0"],
    packages=[],
)
