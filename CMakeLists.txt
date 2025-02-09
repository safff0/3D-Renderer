cmake_minimum_required(VERSION 3.1)
project(3d-renderer)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(src)

add_executable(run_example example.cpp)
target_include_directories(run_example PUBLIC src)
target_link_libraries(run_example PUBLIC 3d_renderer)
