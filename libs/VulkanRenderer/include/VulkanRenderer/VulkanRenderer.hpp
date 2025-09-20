#pragma once
#include "Utils/Logger.hpp"
#include "Windows/IWindow.hpp"
#include "InstanceManager.hpp"
#include "DeviceManager.hpp"
#include "SurfaceManager.hpp"
#include "SwapchainManager.hpp"

class VulkanRenderer{
public:
    VulkanRenderer(Logger& logger): logger_(logger){}
    
    void Initialize(IWindow* window = nullptr);
    void Shutdown();
private:
    Logger& logger_;
    
    bool screenEnabled_ = false;

    std::unique_ptr<InstanceManager> instanceManager_ = nullptr;
    std::unique_ptr<SurfaceManager> surfaceManager_ = nullptr;
    std::unique_ptr<DeviceManager> deviceManager_ = nullptr;
    std::unique_ptr<SwapchainManager> swapchainManager_ = nullptr;
};