#include "VulkanRenderer/VulkanRenderer.hpp"
#include "VulkanRenderer/InstanceManager.hpp"
#include "VulkanRenderer/DeviceManager.hpp"
#include "VulkanRenderer/SurfaceManager.hpp"

void VulkanRenderer::Initialize(IWindow* window) {
    logger_.Log(LogLevel::DEBUG, "InstanceManager : Initialization");
    bool enableScreen = window != nullptr;

    InstanceManager instanceManager = InstanceManager(logger_);
    instanceManager.Initialize(true, enableScreen);
    
    SurfaceManager surfaceManager(logger_);
    surfaceManager.Initialize(instanceManager.GetInstance(), window);

    DeviceManager deviceManager = DeviceManager(logger_);
    deviceManager.Initialize(instanceManager.GetInstance(), surfaceManager.GetSurface(), enableScreen);

    logger_.Log(LogLevel::INFO, "Renderer initialized");

    surfaceManager.Shutdown();
    deviceManager.Shutdown();
    instanceManager.Shutdown();
}