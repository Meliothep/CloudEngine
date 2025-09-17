#include "VulkanRenderer/VulkanRenderer.hpp"

void VulkanRenderer::Initialize(IWindow* window) {
    bool enableScreen = window != nullptr;

    instanceManager_ = std::make_unique<InstanceManager>(logger_);
    instanceManager_->Initialize(true, enableScreen);
    
    surfaceManager_ = std::make_unique<SurfaceManager>(logger_);
    surfaceManager_->Initialize(instanceManager_->GetInstance(), window);

    deviceManager_ = std::make_unique<DeviceManager>(logger_);
    deviceManager_->Initialize(instanceManager_->GetInstance(), surfaceManager_->GetSurface(), enableScreen);

    logger_.Log(LogLevel::INFO, "Renderer initialized");
}

void VulkanRenderer::Shutdown(){
    surfaceManager_->Shutdown();
    surfaceManager_ = nullptr;

    deviceManager_->Shutdown();
    deviceManager_ = nullptr;
    
    instanceManager_->Shutdown();
    instanceManager_ = nullptr;

    logger_.Log(LogLevel::INFO, "Renderer Shutdown");
}