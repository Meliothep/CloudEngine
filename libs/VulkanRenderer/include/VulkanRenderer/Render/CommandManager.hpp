#pragma once
#include "Utils/Logger.hpp"

#include "VulkanRenderer/Ressource/QueueFamilyIndices.hpp"

#include <vulkan/vulkan.h>

class CommandManager {
public:
    CommandManager(Logger& logger) 
        : logger_(logger) {}

    ~CommandManager() = default;

    void Initialize(VkDevice device, QueueFamilyIndices queueFamilyIndices);
    void Shutdown();

    VkCommandPool GetCommandPool() const { return commandPool_; }
    VkCommandBuffer AllocateCommandBuffer(bool primary = true);

    void BeginCommandBuffer(VkCommandBuffer cmdBuffer, VkCommandBufferUsageFlags usage = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    void EndCommandBuffer(VkCommandBuffer cmdBuffer);

private:
    Logger& logger_;
    VkDevice device_{ VK_NULL_HANDLE };
    VkCommandPool commandPool_{ VK_NULL_HANDLE };
};
