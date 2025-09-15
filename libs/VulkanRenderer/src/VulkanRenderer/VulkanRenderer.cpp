#include "VulkanRenderer/VulkanRenderer.hpp"
#include "VulkanRenderer/InstanceManager.hpp"

void VulkanRenderer::Initialize(IWindow* window) {
    logger_.Log(LogLevel::DEBUG, "InstanceManager : Initialization");
    InstanceManager instanceManager = InstanceManager(logger_);
    instanceManager.Initialize(true, window != nullptr);
    
    logger_.Log(LogLevel::INFO, "Renderer initialized");

    instanceManager.Shutdown();
}