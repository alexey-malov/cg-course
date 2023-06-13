cmake_minimum_required(VERSION 3.22)

project(pbr CXX)

find_package(glm)

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
)

target_link_libraries(raytracer PRIVATE glm::glm)
