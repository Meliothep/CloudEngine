project(VkCore LANGUAGES CXX)

find_package(Vulkan REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "include/*.hpp")

add_library(VkCore ${SOURCES} ${HEADERS})

target_include_directories(VkCore
    PUBLIC include
)

target_link_libraries(VkCore PUBLIC 
    Vulkan::Vulkan
)