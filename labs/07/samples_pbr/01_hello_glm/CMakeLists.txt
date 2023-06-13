cmake_minimum_required(VERSION 3.16)

project(01_hello_glm CXX)

set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED True)

if (MSVC)
    # Set warning level 4 + treat warnings as error
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

find_package(glm)

add_executable(${PROJECT_NAME} src/main.cpp)

# Use the global target
target_link_libraries(${PROJECT_NAME} glm::glm)
