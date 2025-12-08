#pragma once
#include "pch.h"

class RenderPass {
public:
    RenderPass(Logger& logger) : logger_(logger) {}
    ~RenderPass() = default;

    void Initialize(VkDevice device, VkFormat swapchainImageFormat);
    void Shutdown();
    
    void BeginRenderPass(VkCommandBuffer cmdBuffer, VkFramebuffer framebuffer, VkExtent2D extent, const VkClearValue* clearValues, uint32_t clearValueCount);
    void EndRenderPass(VkCommandBuffer cmdBuffer);

    VkRenderPass GetRenderPass() const { return renderPass_; }

private:
    Logger& logger_;
    VkDevice device_{ VK_NULL_HANDLE };
    VkRenderPass renderPass_{ VK_NULL_HANDLE };
};
