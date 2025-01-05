cmake_minimum_required(VERSION 2.4)
project(3d-renderer)

find_package(glm CONFIG REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -fsanitize=address,undefined -O2")
set(SRC_LIST src/main.cpp)

add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)