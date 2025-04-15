project(3d-renderer)

add_library(common_lib STATIC
    except.cpp
)

target_include_directories(common_lib PUBLIC ${CMAKE_SOURCE_DIR}/src)
