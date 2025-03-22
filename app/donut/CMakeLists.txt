project(3d-renderer)

add_executable(donut main.cpp application.cpp)

target_include_directories(donut PUBLIC ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/app/common)
target_link_libraries(donut common_lib 3d_renderer)
