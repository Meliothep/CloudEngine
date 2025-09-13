project(Client LANGUAGES CXX)

find_package(unofficial-enet CONFIG REQUIRED)

add_executable(Client src/main.cpp)

target_link_libraries(Client PRIVATE 
    unofficial::enet::enet
    ws2_32
    winmm
)
