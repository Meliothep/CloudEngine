#include "VulkanRenderer/SwapchainFramebufferManager.hpp"

void SwapchainFramebufferManager::Initialize(
    VkDevice device,
    VkRenderPass renderPass,
    const std::vector<std::vector<VkImageView>>& attachmentsPerFramebuffer,
    uint32_t width,
    uint32_t height) 
{
    device_ = device;

    framebuffers_.resize(attachmentsPerFramebuffer.size());

    for (size_t i = 0; i < attachmentsPerFramebuffer.size(); ++i) {
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachmentsPerFramebuffer[i].size());
        framebufferInfo.pAttachments = attachmentsPerFramebuffer[i].data();
        framebufferInfo.width = width;
        framebufferInfo.height = height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers_[i]) != VK_SUCCESS) {
            logger_.Log(LogLevel::CRITICAL, "Failed to create framebuffer!");
            throw std::runtime_error("failed to create framebuffer!");
        }
    }

    logger_.Log(LogLevel::INFO, std::format("Created {} framebuffers", framebuffers_.size()));
}

void SwapchainFramebufferManager::Shutdown() {
    for (auto fb : framebuffers_) {
        if (fb != VK_NULL_HANDLE) {
            vkDestroyFramebuffer(device_, fb, nullptr);
        }
    }
    framebuffers_.clear();
    logger_.Log(LogLevel::INFO, "Destroyed all framebuffers");
}
