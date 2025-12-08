#pragma once
#include "pch.h"

#ifdef _WIN32
    #include <Windows.h>
    #include <vulkan/vulkan_win32.h>
#elif defined(__linux__)
    #include <vulkan/vulkan_xcb.h>
#elif defined(__APPLE__)
    #include <vulkan/vulkan_macos.h>
#endif

class Logger;

class Instance {
public:
    Instance(Logger& logger): logger_(logger) {}
    ~Instance() = default;

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
