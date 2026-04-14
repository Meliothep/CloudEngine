#pragma once
#include "pch.h"

#include "Core/QueueFamilyIndices.hpp"

class Command {
public:
    Command(Logger& logger) : logger_(logger) {}

    void Initialize(VkDevice device, QueueFamilyIndices indices, uint32_t swapchainImageCount);
    void Shutdown();

    void BeginFrame(uint32_t bufferIndex);
    void EndFrame(uint32_t bufferIndex);

    VkCommandBuffer AllocateCommandBuffer(bool primary = true);
    void BeginCommandBuffer(VkCommandBuffer cmd, VkCommandBufferUsageFlags usage = 0);
    void EndCommandBuffer(VkCommandBuffer cmd);
    
    void BindPipeline(uint32_t bufferIndex, VkPipeline pipeline);
    
    VkCommandPool GetCommandPool() const { return commandPool_; }
    VkCommandBuffer GetCommandBuffer(uint32_t index) const { return commandBuffers_[index]; }

    private:
    Logger& logger_;
    VkDevice device_ = VK_NULL_HANDLE;
    VkCommandPool commandPool_ = VK_NULL_HANDLE;

    int bufferIndex_ = 0;
    std::vector<VkCommandBuffer> commandBuffers_;
};
