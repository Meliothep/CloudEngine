#pragma once
#include "Utils/Logger.hpp"
#include "Windows/IWindow.hpp"

#include "Core/InstanceManager.hpp"
#include "Core/DeviceManager.hpp"
#include "Core/SurfaceManager.hpp"

#include "Swapchain/SwapchainManager.hpp"
#include "Swapchain/SwapchainFramebufferManager.hpp"

#include "Render/RenderPassManager.hpp"
#include "Render/CommandManager.hpp"
#include "Render/SyncManager.hpp"
#include "Render/RenderQueue.hpp"

#include "Pipeline/RenderPipelineManager.hpp"

#include <vulkan/vulkan.h>

class VulkanRenderer{
public:
    VulkanRenderer(Logger& logger): logger_(logger){}
    
    void Initialize(IWindow* window = nullptr);
    void Shutdown();

    void DrawFrame();
private:
    Logger& logger_;
    
    bool screenEnabled_ = false;

    std::unique_ptr<InstanceManager> instanceManager_ = nullptr;
    std::unique_ptr<SurfaceManager> surfaceManager_ = nullptr;
    std::unique_ptr<DeviceManager> deviceManager_ = nullptr;
    std::unique_ptr<SwapchainManager> swapchainManager_ = nullptr;
    std::unique_ptr<RenderPassManager> renderPassManager_ = nullptr;
    std::unique_ptr<SwapchainFramebufferManager> swapchainFramebufferManager_ = nullptr;
    std::unique_ptr<RenderPipelineManager> renderPipelineManager_ = nullptr;
    std::unique_ptr<CommandManager> commandManager_ = nullptr;
    std::unique_ptr<SyncManager> syncManager_ = nullptr;
    std::unique_ptr<RenderQueue> renderQueue_ = nullptr;
};