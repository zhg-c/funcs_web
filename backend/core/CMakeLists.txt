cmake_minimum_required(VERSION 3.12)
project(FuncCore LANGUAGES CXX)

# 查找 Pybind11
# Pybind11Config.cmake 脚本将定义 PYBIND11_FOUND, PYBIND11_INCLUDE_DIRS, PYBIND11_LIBRARIES
find_package(pybind11 CONFIG REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LDNS REQUIRED ldns)

# 定义要编译的 C++ 模块
# pybind11_add_module 函数负责链接 Pybind11 库，并设置正确的编译标志
# 模块名 func_core 必须与 PYBIND11_MODULE 中的名称一致
pybind11_add_module(func_core 
bind_core.cpp
func_core.cpp
# 如果有更多 C++ 文件，在这里添加
)
target_link_libraries(func_core PRIVATE ${LDNS_LIBRARIES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# 确保编译后的模块放置在当前目录 (backend/core)
# 这样 Python 才能通过相对导入找到它
set_target_properties(func_core PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
