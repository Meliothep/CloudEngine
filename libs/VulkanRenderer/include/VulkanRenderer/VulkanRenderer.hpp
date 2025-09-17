#pragma once
#include "Utils/Logger.hpp"
#include "Windows/IWindow.hpp"
#include "InstanceManager.hpp"
#include "DeviceManager.hpp"
#include "SurfaceManager.hpp"

class VulkanRenderer{
public:
    VulkanRenderer(Logger& logger): logger_(logger){}
    
    void Initialize(IWindow* window = nullptr);
    void Shutdown();
private:
    Logger& logger_;

    std::unique_ptr<InstanceManager> instanceManager_ = nullptr;
    std::unique_ptr<SurfaceManager> surfaceManager_ = nullptr;
    std::unique_ptr<DeviceManager> deviceManager_ = nullptr;
};