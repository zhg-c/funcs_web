#include "funcs.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>

namespace py = pybind11;

PYBIND11_MODULE(libcore, m)
{
	m.doc() = "C++ 核心功能模块 (libcore)";
	m.def("func_test", &func_test, "A simple test function that returns 'hello,world'.");
}