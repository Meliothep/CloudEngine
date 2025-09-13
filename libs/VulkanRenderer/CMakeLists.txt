project(VulkanRenderer LANGUAGES CXX)

find_package(Vulkan REQUIRED)

add_library(VulkanRenderer
    src/VulkanRenderer.cpp
    include/VulkanRenderer/VulkanRenderer.hpp
)

target_include_directories(VulkanRenderer
    PUBLIC include
)

target_link_libraries(VulkanRenderer PUBLIC 
    Vulkan::Vulkan
)