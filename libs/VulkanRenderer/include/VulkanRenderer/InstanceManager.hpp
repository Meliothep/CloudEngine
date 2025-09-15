#pragma once
#include "Utils/Logger.hpp"

#include <vulkan/vulkan.h>

#ifdef _WIN32
    #include <Windows.h>
    #include <vulkan/vulkan_win32.h>
#elif defined(__linux__)
    #include <vulkan/vulkan_xcb.h> // or vulkan_xlib.h / vulkan_wayland.h
#elif defined(__APPLE__)
    #include <vulkan/vulkan_macos.h>
#endif

#include <vector>
#include <string>

class Logger;

class InstanceManager {
public:
    InstanceManager(Logger& logger): logger_(logger) {}
    ~InstanceManager(){};

    void Initialize(bool enableValidationLayers = true, bool enableScreen = false);
    void Shutdown();

    VkInstance GetInstance() const { return instance_; }

private:
    bool CheckValidationLayerSupport();
    std::vector<const char*> GetRequiredExtensions(bool enableValidationLayers, bool enableSurface);

private:
    Logger& logger_;
    
    VkInstance instance_{VK_NULL_HANDLE};
    bool validationEnabled_{false};

    VkDebugUtilsMessengerEXT debugMessenger_{VK_NULL_HANDLE};
};
