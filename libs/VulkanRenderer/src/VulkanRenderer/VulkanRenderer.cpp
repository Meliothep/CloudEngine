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
    commandManager_->Initialize(deviceManager_->GetDevice(), deviceManager_->GetQueueFamilyIndices(), swapchainManager_->GetImageViews().size());

    syncManager_ = std::make_unique<SyncManager>(logger_);
    syncManager_->Initialize(deviceManager_->GetDevice());

    renderQueue_ = std::make_unique<RenderQueue>();


    logger_.Log(LogLevel::INFO, "Renderer initialized");
}

void VulkanRenderer::Shutdown(){
    syncManager_->WaitForFence();

    syncManager_->Shutdown();
    syncManager_ = nullptr;

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

void VulkanRenderer::DrawFrame() {
    // Wait for previous frame
    syncManager_->WaitForFence();

    // Acquire image
    uint32_t imageIndex;
    vkAcquireNextImageKHR(deviceManager_->GetDevice(),
                          swapchainManager_->GetSwapchain(),
                          UINT64_MAX,
                          syncManager_->GetImageAvailableSemaphore(),
                          VK_NULL_HANDLE,
                          &imageIndex);

    // Begin command buffer for this frame
    VkCommandBuffer cmd = commandManager_->BeginFrame(imageIndex);

    // Begin render pass (main pass)
    VkRenderPassBeginInfo rpInfo{};
    rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpInfo.renderPass = renderPassManager_->GetRenderPass();
    rpInfo.framebuffer = swapchainFramebufferManager_->GetFramebuffers()[imageIndex];
    rpInfo.renderArea.offset = {0,0};
    rpInfo.renderArea.extent = swapchainManager_->GetExtent();

    VkClearValue clearColor{};
    clearColor.color = {0.1f, 0.1f, 0.1f, 1.0f};
    rpInfo.clearValueCount = 1;
    rpInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Record all submitted external commands from the RenderQueue
    for (auto* command : renderQueue_->GetCommands()) {
        command->Record(cmd);
    }

    vkCmdEndRenderPass(cmd);

    commandManager_->EndFrame(cmd);

    // Submit
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { syncManager_->GetImageAvailableSemaphore() };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;

    VkSemaphore signalSemaphores[] = { syncManager_->GetRenderFinishedSemaphore() };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    // Submit to graphics queue (renderer owns queue handles)
    vkQueueSubmit(deviceManager_->GetGraphicsQueue(), 1, &submitInfo, syncManager_->GetFence());

    // Present
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapchains[] = { swapchainManager_->GetSwapchain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;
    vkQueuePresentKHR(deviceManager_->GetPresentQueue(), &presentInfo);

    // Clear the render queue for next frame (ownership of commands is external)
    renderQueue_->Clear();
}
