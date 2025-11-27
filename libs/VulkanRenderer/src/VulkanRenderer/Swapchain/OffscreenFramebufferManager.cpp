#include "VulkanRenderer/Swapchain/OffscreenFramebufferManager.hpp"


void OffscreenFramebufferManager::Initialize(
    VkDevice device,
    VkRenderPass renderPass,
    const std::vector<VkImageView>& attachments,
    uint32_t width,
    uint32_t height) 
{
    device_ = device;

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = width;
    framebufferInfo.height = height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer_) != VK_SUCCESS) {
        logger_.Log(LogLevel::CRITICAL, "Failed to create framebuffer!");
        throw std::runtime_error("failed to create framebuffer!");
    }

    logger_.Log(LogLevel::INFO, "Framebuffer created successfully");
}

void OffscreenFramebufferManager::Shutdown() {
    if (framebuffer_ != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(device_, framebuffer_, nullptr);
        framebuffer_ = VK_NULL_HANDLE;
        logger_.Log(LogLevel::INFO, "Framebuffer destroyed successfully");
    }
}
