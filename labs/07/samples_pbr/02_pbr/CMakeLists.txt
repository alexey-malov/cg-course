cmake_minimum_required(VERSION 3.22c)

project(pbr CXX)

find_package(glm)
find_package(sdl)

set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED true)

if (MSVC)
    # Set warning level 4 + treat warnings as error
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()


add_executable(raytracer
    src/main.cpp
    src/core/config.hpp
    src/core/math.hpp
    src/core/point3_io.hpp
    src/core/point3.hpp
    src/core/ray.hpp
    src/core/ray_io.hpp
    src/core/vector2.hpp
    src/core/vector3.hpp
    src/core/vector3_io.hpp
)

target_link_libraries(raytracer PRIVATE glm::glm)
target_link_libraries(raytracer PRIVATE sdl::sdl)