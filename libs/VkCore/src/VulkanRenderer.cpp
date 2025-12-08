#include "VulkanRenderer.hpp"

void VulkanRenderer::Initialize(IWindow* window) {
    window_ = window;
    
    bool screenEnabled = window != nullptr;

    instance_ = std::make_unique<Instance>(logger_);
    instance_->Initialize(true, screenEnabled);
    
    surface_ = std::make_unique<Surface>(logger_);
    surface_->Initialize(instance_->GetInstance(), window);

    device_ = std::make_unique<Device>(logger_);
    device_->Initialize(instance_->GetInstance(), surface_->GetSurface(), screenEnabled);
    
    if(screenEnabled){
        swapchain_ = std::make_unique<Swapchain>(logger_);
        swapchain_->Initialize(device_->GetPhysicalDevice(), 
            device_->GetDevice(), 
            surface_->GetSurface(), 
            device_->GetQueueFamilyIndices(),
            window
        );    

        renderPass_ = std::make_unique<RenderPass>(logger_);
        renderPass_->Initialize( 
            device_->GetDevice(), 
            swapchain_->GetImageFormat()
        );
        
        renderPipeline_ = std::make_unique<RenderPipeline>(logger_);
        renderPipeline_->Initialize(
            device_->GetDevice(),
            renderPass_->GetRenderPass(),
            "triangle.vert",
            "triangle.frag",
            swapchain_->GetExtent()
        );

        std::vector<std::vector<VkImageView>> attachmentsPerFramebuffer;
        for (auto& imageView : swapchain_->GetImageViews()) {
            attachmentsPerFramebuffer.push_back({ imageView });
        }

        swapchainFramebuffer_ = std::make_unique<SwapchainFramebuffer>(logger_);
        swapchainFramebuffer_->Initialize(
            device_->GetDevice(),
            renderPass_->GetRenderPass(),
            attachmentsPerFramebuffer, 
            window->GetWidth(),
            window->GetHeight()
        );
    }else{

    }
    
    command_ =  std::make_unique<Command>(logger_);
    command_->Initialize(device_->GetDevice(), device_->GetQueueFamilyIndices(), swapchain_->GetImageViews().size());

    sync_ = std::make_unique<Sync>(logger_);
    sync_->Initialize(device_->GetDevice(), swapchain_->GetImageViews().size());

    renderQueue_ = std::make_unique<RenderQueue>();

    // Create a simple triangle mesh to draw
    mesh_ = std::make_unique<Mesh>(logger_);
    std::vector<Vertex> vertices{
        Vertex{{0.0f, -0.5f, 0.0f}, {0.0f,0.0f,1.0f}, {0.5f, 1.0f}}, // TOP
        Vertex{{-0.5f, 0.5f, 0.0f}, {0.0f,1.0f,0.0f}, {0.0f, 0.0f}}, // RIGHT
        Vertex{{0.5f, 0.5f, 0.0f},  {1.0f,0.0f,0.0f}, {1.0f, 0.0f}}, // LEFT
    };

    mesh_->Initialize(
        device_->GetDevice(),
        device_->GetPhysicalDevice(),
        device_->GetGraphicsQueue(),
        command_->GetCommandPool(),
        vertices
    );


    logger_.Log(LogLevel::INFO, "Renderer initialized");
}

void VulkanRenderer::Shutdown(){
    vkDeviceWaitIdle(device_->GetDevice());

    sync_->WaitForFence();

    sync_->Shutdown();
    sync_ = nullptr;

    command_->Shutdown();
    command_ = nullptr;

    renderPipeline_->Shutdown();
    renderPipeline_ = nullptr;

    mesh_->Shutdown();
    mesh_.reset();

    swapchainFramebuffer_->Shutdown();
    swapchainFramebuffer_ = nullptr; 

    renderPass_->Shutdown();
    renderPass_ = nullptr;

    if( window_ != nullptr){
        swapchain_->Shutdown();
        swapchain_ = nullptr;
    }

    surface_->Shutdown();
    surface_ = nullptr;

    device_->Shutdown();
    device_ = nullptr;
    
    instance_->Shutdown();
    instance_ = nullptr;

    logger_.Log(LogLevel::INFO, "Renderer Shutdown");
}

void VulkanRenderer::HandleResize(){
    vkDeviceWaitIdle(device_->GetDevice());

    swapchain_->Shutdown();

    swapchain_->Initialize(device_->GetPhysicalDevice(), 
        device_->GetDevice(), 
        surface_->GetSurface(), 
        device_->GetQueueFamilyIndices(),
        window_
    );    
    
    std::vector<std::vector<VkImageView>> attachmentsPerFramebuffer;
    for (auto& imageView : swapchain_->GetImageViews()) {
        attachmentsPerFramebuffer.push_back({ imageView });
    }
    swapchainFramebuffer_->Shutdown();

    swapchainFramebuffer_->Initialize(
        device_->GetDevice(),
        renderPass_->GetRenderPass(),
        attachmentsPerFramebuffer, 
        window_->GetWidth(),
        window_->GetHeight()
    );
}

void VulkanRenderer::DrawFrame() {
    // Wait for previous frame
    sync_->WaitForFence();
    sync_->AdvanceFrame();
    
    // Acquire image
    uint32_t imageIndex;
    vkAcquireNextImageKHR(device_->GetDevice(),
                          swapchain_->GetSwapchain(),
                          UINT64_MAX,
                          sync_->GetImageAvailableSemaphore(),
                          VK_NULL_HANDLE,
                          &imageIndex);

    VkCommandBuffer cmd = command_->BeginFrame(imageIndex);

    VkRenderPassBeginInfo rpInfo{};
    rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpInfo.renderPass = renderPass_->GetRenderPass();
    rpInfo.framebuffer = swapchainFramebuffer_->GetFramebuffers()[imageIndex];
    rpInfo.renderArea.offset = {0,0};
    rpInfo.renderArea.extent = swapchain_->GetExtent();

    VkClearValue clearColor{};
    clearColor.color = {0.f, 0.f, 0.f, 1.0f};
    rpInfo.clearValueCount = 1;
    rpInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Bind our graphics pipeline and draw the triangle mesh if present
    if (renderPipeline_ && mesh_) {
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, renderPipeline_->GetPipeline());
        // Provide dynamic viewport and scissor if the pipeline expects them
        VkExtent2D extent = swapchain_->GetExtent();
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(extent.width);
        viewport.height = static_cast<float>(extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(cmd, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = extent;
        vkCmdSetScissor(cmd, 0, 1, &scissor);
        
        mesh_->Bind(cmd);

        vkCmdDraw(cmd, 3, 1, 0, 0);
    }

    for (auto* command : renderQueue_->GetCommands()) {
        command->Record(cmd);
    }

    vkCmdEndRenderPass(cmd);

    command_->EndFrame(cmd);

    // Submit
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { sync_->GetImageAvailableSemaphore() };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;

    VkSemaphore signalSemaphores[] = { sync_->GetRenderFinishedSemaphore() };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    // Submit to graphics queue (renderer owns queue handles)
    vkQueueSubmit(device_->GetGraphicsQueue(), 1, &submitInfo, sync_->GetFence());

    // Present
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapchains[] = { swapchain_->GetSwapchain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;
    vkQueuePresentKHR(device_->GetPresentQueue(), &presentInfo);

    // Clear the render queue for next frame (ownership of commands is external)
    renderQueue_->Clear();
}
