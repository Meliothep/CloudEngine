project(Server LANGUAGES CXX)

find_package(unofficial-enet CONFIG REQUIRED)  # ENet via vcpkg

add_executable(Server src/main.cpp)

target_link_libraries(Server PRIVATE 
    VulkanRenderer
    unofficial::enet::enet
    ws2_32
    winmm
)
