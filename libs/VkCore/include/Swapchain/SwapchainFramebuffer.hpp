#pragma once
#include "pch.h"

class SwapchainFramebuffer {
public:
    SwapchainFramebuffer(Logger& logger) : logger_(logger) {}
    ~SwapchainFramebuffer() = default;

    void Initialize(VkDevice device,
                    VkRenderPass renderPass,
                    const std::vector<std::vector<VkImageView>>& attachmentsPerFramebuffer,
                    uint32_t width,
                    uint32_t height);

    void Shutdown();

    const std::vector<VkFramebuffer>& GetFramebuffers() const { return framebuffers_; }

private:
    Logger& logger_;
    VkDevice device_{VK_NULL_HANDLE};
    std::vector<VkFramebuffer> framebuffers_;
};
