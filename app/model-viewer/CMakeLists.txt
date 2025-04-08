project(3d-renderer)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(model-viewer main.cpp application.cpp)

target_include_directories(model-viewer PUBLIC ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/app/common)
target_link_libraries(model-viewer common_lib 3d_renderer SFML::Graphics)
