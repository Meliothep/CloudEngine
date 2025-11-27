#include "VulkanRenderer/Swapchain/SwapchainFramebufferManager.hpp"

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

        VkResult res = vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers_[i]);
        if (res != VK_SUCCESS) {
            logger_.Log(LogLevel::CRITICAL, std::string("Failed to create framebuffer! VkResult=") + std::to_string(static_cast<int>(res)));
            throw std::runtime_error("failed to create framebuffer!");
        }
    }

    logger_.Log(LogLevel::INFO, std::string("Created ") + std::to_string(framebuffers_.size()) + std::string(" framebuffers"));
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
