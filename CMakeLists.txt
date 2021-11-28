cmake_minimum_required(VERSION 3.17)
project(Storm)
set(CMAKE_CXX_STANDARD 17)
add_executable(Storm main.cpp)

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
target_link_libraries(Storm ${PYTHON_LIBRARIES})