project(3d-renderer)

find_package(glm CONFIG REQUIRED)

include(sources.cmake)
target_link_libraries(3d_renderer PUBLIC glm::glm)