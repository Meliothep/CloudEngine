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
    
    renderPassManager_ = std::make_unique<RenderPassManager>(logger_);
    renderPassManager_->Initialize( 
        deviceManager_->GetDevice(), 
        swapchainManager_->GetImageFormat()
    );
    
    renderPipelineManager_ = std::make_unique<RenderPipelineManager>(logger_);
    renderPipelineManager_->Initialize(
        deviceManager_->GetDevice(),
        renderPassManager_->GetRenderPass(),
        "triangle.vert",
        "triangle.frag",
        swapchainManager_->GetExtent()
    );

    std::vector<std::vector<VkImageView>> attachmentsPerFramebuffer;
    for (auto& imageView : swapchainManager_->GetImageViews()) {
        attachmentsPerFramebuffer.push_back({ imageView });
    }

    swapchainFramebufferManager_ = std::make_unique<SwapchainFramebufferManager>(logger_);
    swapchainFramebufferManager_->Initialize(
        deviceManager_->GetDevice(),
        renderPassManager_->GetRenderPass(),
        attachmentsPerFramebuffer, 
        window->GetWidth(),
        window->GetHeight()
    );

    commandManager_ =  std::make_unique<CommandManager>(logger_);
    commandManager_->Initialize(deviceManager_->GetDevice(), deviceManager_->GetQueueFamilyIndices());

    logger_.Log(LogLevel::INFO, "Renderer initialized");
}

void VulkanRenderer::Shutdown(){
    commandManager_->Shutdown();
    commandManager_ = nullptr;

    renderPipelineManager_->Shutdown();
    renderPipelineManager_ = nullptr;

    swapchainFramebufferManager_->Shutdown();
    swapchainFramebufferManager_ = nullptr; 

    renderPassManager_->Shutdown();
    renderPassManager_ = nullptr;

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

void VulkanRenderer::RecordSimpleDraw() {
    if (!renderPassManager_) {
        logger_.Log(LogLevel::EXCEPT, "RenderPassManager not initialized (RecordSimpleDraw)");
        throw std::runtime_error("RenderPassManager not initialized");
    }
    if (!swapchainFramebufferManager_) {
        logger_.Log(LogLevel::EXCEPT, "SwapchainFramebufferManager not initialized (RecordSimpleDraw)");
        throw std::runtime_error("SwapchainFramebufferManager not initialized");
    }
    if (!swapchainManager_) {
        logger_.Log(LogLevel::EXCEPT, "SwapchainManager not initialized (RecordSimpleDraw)");
        throw std::runtime_error("SwapchainManager not initialized");
    }
    if (!renderPipelineManager_) {
        logger_.Log(LogLevel::EXCEPT, "RenderPipelineManager not initialized (RecordSimpleDraw)");
        throw std::runtime_error("RenderPipelineManager not initialized");
    }

    const auto& framebuffers = swapchainFramebufferManager_->GetFramebuffers();
    if (framebuffers.empty()) {
        logger_.Log(LogLevel::EXCEPT, "No framebuffers available (RecordSimpleDraw)");
        throw std::runtime_error("No framebuffers available");
    }

    VkFramebuffer framebuffer = framebuffers[0];
    VkExtent2D extent = swapchainManager_->GetExtent();
    VkPipeline pipeline = renderPipelineManager_->GetPipeline();

    VkClearValue clearColor{};
    clearColor.color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    VkCommandBuffer cmdBuffer = commandManager_->AllocateCommandBuffer();
    commandManager_->BeginCommandBuffer(cmdBuffer);

    // Begin the render pass using the RenderPassManager helper
    renderPassManager_->BeginRenderPass(cmdBuffer, framebuffer, extent, &clearColor, 1);

    // Bind the graphics pipeline
    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    // Set viewport
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

    // Set scissor
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = extent;
    vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

    // Issue a simple draw (3 vertices)
    vkCmdDraw(cmdBuffer, 3, 1, 0, 0);

    // End render pass
    renderPassManager_->EndRenderPass(cmdBuffer);
    commandManager_->EndCommandBuffer(cmdBuffer);
}