#pragma once
#include "Utils/Logger.hpp"
#include "Windows/IWindow.hpp"

#include <vulkan/vulkan.h>

#ifdef _WIN32
    #include <Windows.h>
    #include <vulkan/vulkan_win32.h>
#elif defined(__linux__)
    #include <vulkan/vulkan_xcb.h> // or vulkan_xlib.h / vulkan_wayland.h
#elif defined(__APPLE__)
    #include <vulkan/vulkan_macos.h>
#endif

class SurfaceManager {
public:
    SurfaceManager(Logger& logger) : logger_(logger) {}
    ~SurfaceManager() { Shutdown(); }

    void Initialize(VkInstance instance, IWindow* window);
    void Shutdown();

    VkSurfaceKHR GetSurface() const { return surface_; }

private:
    Logger& logger_;
    VkInstance instance_{VK_NULL_HANDLE};
    VkSurfaceKHR surface_{VK_NULL_HANDLE};
};