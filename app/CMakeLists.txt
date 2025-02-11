project(3d-renderer)

include_directories(../src)

add_library(app
    application.cpp
    except.cpp
)

target_link_libraries(app PUBLIC 3d_renderer)
