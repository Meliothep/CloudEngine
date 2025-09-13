cmake_minimum_required(VERSION 3.22)

project(CloudEngine LANGUAGES CXX)

# Forcer le C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Utiliser vcpkg
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

# Inclure les sous-projets
add_subdirectory(libs/VulkanRenderer)
add_subdirectory(apps/RendererTest)
add_subdirectory(apps/Server)
add_subdirectory(apps/Client)
