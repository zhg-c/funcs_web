cmake_minimum_required(VERSION 3.12)
project(PortScannerCore LANGUAGES CXX)

# 查找 Pybind11
# Pybind11Config.cmake 脚本将定义 PYBIND11_FOUND, PYBIND11_INCLUDE_DIRS, PYBIND11_LIBRARIES
find_package(pybind11 CONFIG REQUIRED)

# 定义要编译的 C++ 模块
# pybind11_add_module 函数负责链接 Pybind11 库，并设置正确的编译标志
# 模块名 port_scanner_core 必须与 PYBIND11_MODULE 中的名称一致
pybind11_add_module(port_scanner_core 
    port_scanner.cpp
    # 如果有更多 C++ 文件，在这里添加
)

# 确保编译后的模块放置在当前目录 (backend/core)
# 这样 Python 才能通过相对导入找到它
set_target_properties(port_scanner_core PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
