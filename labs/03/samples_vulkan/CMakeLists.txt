cmake_minimum_required(VERSION 3.25)
project(VulkanCourse_Part1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(VKC_WARNINGS_AS_ERRORS "Считать предупреждения ошибками" ON)

set(FETCHCONTENT_UPDATES_DISCONNECTED ON CACHE BOOL "" FORCE)

if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

if(MSVC)
    string(REGEX REPLACE "[-/]W[0-4]" "" CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}")
    string(REGEX REPLACE "[-/]W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

find_package(Vulkan REQUIRED)
get_filename_component(VKC_SDK_ROOT "${Vulkan_INCLUDE_DIR}" DIRECTORY)

set(VKC_SDK_LIB_DIR "${VKC_SDK_ROOT}/Lib")

find_library(VKC_SHADERC_LIBRARY
    NAMES shaderc_shared
    HINTS "${VKC_SDK_LIB_DIR}"
    NO_DEFAULT_PATH)

if(NOT VKC_SHADERC_LIBRARY)
    message(FATAL_ERROR "shaderc_shared не найден в ${VKC_SDK_LIB_DIR}")
endif()

include(FetchContent)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)

FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
    GIT_SHALLOW    TRUE
    SYSTEM EXCLUDE_FROM_ALL)

FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
    GIT_SHALLOW    TRUE
    SOURCE_SUBDIR  "do-not-configure")

FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.8
    GIT_SHALLOW    TRUE)

FetchContent_MakeAvailable(glfw glm imgui)

add_library(glm INTERFACE)
add_library(glm::glm ALIAS glm)
target_include_directories(glm SYSTEM INTERFACE "${glm_SOURCE_DIR}")

add_library(imgui STATIC
    "${imgui_SOURCE_DIR}/imgui.cpp"
    "${imgui_SOURCE_DIR}/imgui_draw.cpp"
    "${imgui_SOURCE_DIR}/imgui_tables.cpp"
    "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp")

target_include_directories(imgui SYSTEM PUBLIC
    "${imgui_SOURCE_DIR}"
    "${imgui_SOURCE_DIR}/backends")

target_link_libraries(imgui PUBLIC Vulkan::Vulkan glfw)

if(MSVC)
    get_target_property(glfw_options glfw COMPILE_OPTIONS)
    if(glfw_options)
        list(REMOVE_ITEM glfw_options "/W3")
        set_target_properties(glfw PROPERTIES COMPILE_OPTIONS "${glfw_options}")
    endif()

    target_compile_options(glfw  PRIVATE /W0)
    target_compile_options(imgui PRIVATE /W0)
endif()

add_library(vkc_warnings INTERFACE)

if(MSVC)
    target_compile_options(vkc_warnings INTERFACE /W4 /permissive- /external:W0)
    if(VKC_WARNINGS_AS_ERRORS)
        target_compile_options(vkc_warnings INTERFACE /WX)
    endif()
else()
    target_compile_options(vkc_warnings INTERFACE -Wall -Wextra -Wpedantic)
    if(VKC_WARNINGS_AS_ERRORS)
        target_compile_options(vkc_warnings INTERFACE -Werror)
    endif()
endif()

add_subdirectory(VulkanCore)

# vkc_add_example(<цель> SOURCES <файлы...>)
# Линкует ядро, кладёт рядом с exe содержимое shaders/ и assets/.
function(vkc_add_example name)
    cmake_parse_arguments(ARG "" "" "SOURCES" ${ARGN})

    add_executable(${name} ${ARG_SOURCES})
    target_link_libraries(${name} PRIVATE VulkanCore vkc_warnings)

    set_target_properties(${name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY      "${CMAKE_BINARY_DIR}/bin/${name}"
        VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${name}>"
        FOLDER "Examples")

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
        add_custom_command(TARGET ${name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${CMAKE_CURRENT_SOURCE_DIR}/shaders" "$<TARGET_FILE_DIR:${name}>")
    endif()

    add_custom_command(TARGET ${name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:${name}>")
endfunction()

add_subdirectory("00 Hello Triangle")
add_subdirectory("01 Hello Triangle Buffer")
add_subdirectory("02 Hello Triangle Drag")
add_subdirectory("03 Viewport Test")
add_subdirectory("04 Animate")
add_subdirectory("05 Orbit Calculation")
