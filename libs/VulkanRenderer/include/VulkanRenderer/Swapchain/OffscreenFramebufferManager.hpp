#pragma once
#include "pch.h"

class OffscreenFramebufferManager {
public:
    OffscreenFramebufferManager(Logger& logger) : logger_(logger) {}
    ~OffscreenFramebufferManager() = default;

    void Initialize(VkDevice device,
                    VkRenderPass renderPass,
                    const std::vector<VkImageView>& attachments,
                    uint32_t width,
                    uint32_t height);

    void Shutdown();

    VkFramebuffer GetFramebuffer() const { return framebuffer_; }

private:
    Logger& logger_;
    VkDevice device_{VK_NULL_HANDLE};
    VkFramebuffer framebuffer_{VK_NULL_HANDLE};
};
