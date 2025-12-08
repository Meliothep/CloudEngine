#pragma once
#include "pch.h"

#include "Core/IWindow.hpp"
#include "Core/Instance.hpp"
#include "Core/Device.hpp"
#include "Core/Surface.hpp"
#include "Core/Command.hpp"
#include "Core/Sync.hpp"

#include "Swapchain/Swapchain.hpp"
#include "Swapchain/SwapchainFramebuffer.hpp"

#include "Pipeline/RenderPass.hpp"
#include "Pipeline/RenderPipeline.hpp"

#include "Rendering/RenderQueue.hpp"

#include "Resources/Mesh.hpp"

class VulkanRenderer{
public:
    VulkanRenderer(Logger& logger): logger_(logger){}
    
    void Initialize(IWindow* window = nullptr);
    void Shutdown();

    void HandleResize();
    void DrawFrame();
private:
    Logger& logger_;
    
    IWindow* window_;

    std::unique_ptr<Instance> instance_ = nullptr;
    std::unique_ptr<Surface> surface_ = nullptr;
    std::unique_ptr<Device> device_ = nullptr;
    std::unique_ptr<Swapchain> swapchain_ = nullptr;
    std::unique_ptr<RenderPass> renderPass_ = nullptr;
    std::unique_ptr<SwapchainFramebuffer> swapchainFramebuffer_ = nullptr;
    std::unique_ptr<RenderPipeline> renderPipeline_ = nullptr;
    std::unique_ptr<Command> command_ = nullptr;
    std::unique_ptr<Sync> sync_ = nullptr;
    std::unique_ptr<RenderQueue> renderQueue_ = nullptr;
    std::unique_ptr<Mesh> mesh_ = nullptr;
};