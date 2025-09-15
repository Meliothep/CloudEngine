project(VulkanRenderer LANGUAGES CXX)

find_package(Vulkan REQUIRED)

file(GLOB SOURCES "src/*/*.cpp")
file(GLOB HEADERS "include/*/*.h" "include/*/*.hpp")

add_library(VulkanRenderer ${SOURCES} ${HEADERS})

target_include_directories(VulkanRenderer
    PUBLIC include
)

target_link_libraries(VulkanRenderer PUBLIC 
    Vulkan::Vulkan
)