#include "VulkanRenderer/VulkanRenderer.hpp"

void VulkanRenderer::Initialize(IWindow* window) {
    screenEnabled_ = window != nullptr;

    instanceManager_ = std::make_unique<InstanceManager>(logger_);
    instanceManager_->Initialize(true, screenEnabled_);
    
    surfaceManager_ = std::make_unique<SurfaceManager>(logger_);
    surfaceManager_->Initialize(instanceManager_->GetInstance(), window);

    deviceManager_ = std::make_unique<DeviceManager>(logger_);
    deviceManager_->Initialize(instanceManager_->GetInstance(), surfaceManager_->GetSurface(), screenEnabled_);
    
    if(screenEnabled_){
        swapchainManager_ = std::make_unique<SwapchainManager>(logger_);
        swapchainManager_->Initialize(deviceManager_->GetPhysicalDevice(), 
            deviceManager_->GetDevice(), 
            surfaceManager_->GetSurface(), 
            deviceManager_->GetQueueFamilyIndices(),
            window
        );    
    }else{

    }

    logger_.Log(LogLevel::INFO, "Renderer initialized");
}

void VulkanRenderer::Shutdown(){

    if(screenEnabled_){
        swapchainManager_->Shutdown();
        swapchainManager_ = nullptr;
    }

    surfaceManager_->Shutdown();
    surfaceManager_ = nullptr;

    deviceManager_->Shutdown();
    deviceManager_ = nullptr;
    
    instanceManager_->Shutdown();
    instanceManager_ = nullptr;

    logger_.Log(LogLevel::INFO, "Renderer Shutdown");
}