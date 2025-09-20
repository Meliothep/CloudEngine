#pragma once
#include "Utils/Logger.hpp"
#include "DeviceManager.hpp"

#include <vulkan/vulkan.h>
#include <vector>

class CommandManager {
public:
    CommandManager(Logger& logger, DeviceManager& deviceManager) 
        : logger_(logger), deviceManager_(deviceManager) {}

    ~CommandManager() = default;

    void Initialize();
    void Shutdown();

    VkCommandPool GetCommandPool() const { return commandPool_; }
    VkCommandBuffer AllocateCommandBuffer(bool primary = true);

    void BeginCommandBuffer(VkCommandBuffer cmdBuffer, VkCommandBufferUsageFlags usage = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    void EndCommandBuffer(VkCommandBuffer cmdBuffer);

private:
    Logger& logger_;
    DeviceManager& deviceManager_;

    VkCommandPool commandPool_{ VK_NULL_HANDLE };
};
