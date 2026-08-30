add_library(VulkanCore STATIC
    src/Window.cpp
    src/GpuMemory.cpp
    src/RenderDevice.cpp
    src/Buffer.cpp
    src/ShaderCache.cpp
    src/Geometry.cpp
    src/PipelineDesc.cpp
    src/Pipeline.cpp
    src/Rendering.cpp
    src/FpsCounter.cpp
    src/Application.cpp
    src/internal/Swapchain.cpp
    src/internal/FrameLoop.cpp
    src/internal/Gui.cpp
    src/GUI/LabInfo.cpp)

target_include_directories(VulkanCore
    PUBLIC  include
    PRIVATE src)

target_include_directories(VulkanCore SYSTEM PRIVATE "${VKC_SDK_ROOT}/Include")

target_link_libraries(VulkanCore
    PUBLIC  Vulkan::Vulkan glfw glm::glm imgui
    PRIVATE "${VKC_SHADERC_LIBRARY}"
            vkc_warnings)

target_compile_definitions(VulkanCore PUBLIC
    VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
    GLFW_INCLUDE_NONE
    GLM_FORCE_DEPTH_ZERO_TO_ONE
    GLM_ENABLE_EXPERIMENTAL
    NOMINMAX
    WIN32_LEAN_AND_MEAN)
