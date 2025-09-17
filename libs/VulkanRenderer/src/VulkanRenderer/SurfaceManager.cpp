#include "VulkanRenderer/SurfaceManager.hpp"

void SurfaceManager::Initialize(VkInstance instance, IWindow* window){
    instance_ = instance;

    #ifdef _WIN32
        VkWin32SurfaceCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hinstance = GetModuleHandle(nullptr);
        createInfo.hwnd = reinterpret_cast<HWND>(window->GetNativeHandle());

        if (vkCreateWin32SurfaceKHR(instance_, &createInfo, nullptr, &surface_) != VK_SUCCESS) {
            logger_.Log(LogLevel::EXCEPT, "Failed to create Win32 surface!");
            throw std::runtime_error("Failed to create Win32 surface!");
        }
    #elif defined(__linux__)
        /*
        VkXcbSurfaceCreateInfoKHR createInfo{};

        createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
        createInfo.connection = window->GetXcbConnection();  // must be provided by your IWindow
        createInfo.window = window->GetXcbWindow();

        if (vkCreateXcbSurfaceKHR(instanceManager.GetInstance(), &createInfo, nullptr, &surface_) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create XCB surface!");
        }
        */
    #elif defined(__APPLE__)
        /*
        VkMacOSSurfaceCreateInfoMVK createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
        createInfo.pView = window->GetNSView(); // from your Cocoa/Obj-C layer

        if (vkCreateMacOSSurfaceMVK(instanceManager.GetInstance(), &createInfo, nullptr, &surface_) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create macOS surface!");
        }
        */
    #endif    

    logger_.Log(LogLevel::INFO, "Surface created ");
}

void SurfaceManager::Shutdown() {
    if (surface_ != VK_NULL_HANDLE && instance_ != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(instance_, surface_, nullptr);
        surface_ = VK_NULL_HANDLE;
        logger_.Log(LogLevel::INFO, "Surface destroyed successfully");
    }
}
