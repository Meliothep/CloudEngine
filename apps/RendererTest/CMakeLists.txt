project(RendererTest LANGUAGES CXX)

add_executable(RendererTest src/main.cpp)

target_link_libraries(RendererTest
    PRIVATE VulkanRenderer
)
