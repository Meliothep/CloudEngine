#include "VulkanRenderer/Render/CommandManager.hpp"

VkCommandBuffer CommandManager::AllocateCommandBuffer(bool primary){
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool_;
    allocInfo.level = primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    if (vkAllocateCommandBuffers(device_, &allocInfo, &commandBuffer) != VK_SUCCESS) {
        logger_.Log(LogLevel::EXCEPT, "Failed to allocate command buffer!");
        throw std::runtime_error("Failed to allocate command buffer!");
    }

    return commandBuffer;
}

void CommandManager::BeginCommandBuffer(VkCommandBuffer cmdBuffer, VkCommandBufferUsageFlags usage){
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = usage;
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(cmdBuffer, &beginInfo) != VK_SUCCESS) {
        logger_.Log(LogLevel::EXCEPT, "Failed to begin recording command buffer!");
        throw std::runtime_error("Failed to begin recording command buffer!");
    }
}

void CommandManager::EndCommandBuffer(VkCommandBuffer cmdBuffer){
    if (vkEndCommandBuffer(cmdBuffer) != VK_SUCCESS) {
        logger_.Log(LogLevel::EXCEPT, "Failed to record command buffer!");
        throw std::runtime_error("Failed to record command buffer!");
    }
}

void CommandManager::Initialize(VkDevice device, QueueFamilyIndices queueFamilyIndices){
    device_ = device;

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool_) != VK_SUCCESS) {
        logger_.Log(LogLevel::EXCEPT, "Failed to create command pool!");
        throw std::runtime_error("failed to create command pool!");
    }

    logger_.Log(LogLevel::INFO, "Command pool created successfully");
}

void CommandManager::Shutdown(){
     if (commandPool_ != VK_NULL_HANDLE) {
        vkDestroyCommandPool(device_, commandPool_, nullptr);
        commandPool_ = VK_NULL_HANDLE;
    }
    logger_.Log(LogLevel::INFO, "Command pool destroyed successfully");
}