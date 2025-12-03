project(VulkanRenderer LANGUAGES CXX)

find_package(Vulkan REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "include/*.hpp")

add_library(VulkanRenderer ${SOURCES} ${HEADERS})

target_include_directories(VulkanRenderer
    PUBLIC include
)

target_precompile_headers(VulkanRenderer PRIVATE "include/pch.h")

target_link_libraries(VulkanRenderer PUBLIC 
    Vulkan::Vulkan
)