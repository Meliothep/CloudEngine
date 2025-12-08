#pragma once
#include "pch.h"

#include "Core/QueueFamilyIndices.hpp"

class Command {
public:
    Command(Logger& logger) : logger_(logger) {}

    void Initialize(VkDevice device, QueueFamilyIndices indices, uint32_t swapchainImageCount);
    void Shutdown();

    VkCommandBuffer BeginFrame(uint32_t imageIndex);
    void EndFrame(VkCommandBuffer cmd);

    VkCommandBuffer AllocateCommandBuffer(bool primary = true);
    void BeginCommandBuffer(VkCommandBuffer cmd, VkCommandBufferUsageFlags usage = 0);
    void EndCommandBuffer(VkCommandBuffer cmd);
    VkCommandPool GetCommandPool() const { return commandPool_; }

private:
    Logger& logger_;
    VkDevice device_ = VK_NULL_HANDLE;
    VkCommandPool commandPool_ = VK_NULL_HANDLE;

    std::vector<VkCommandBuffer> commandBuffers_;
};
