#include "Core/Command.hpp"

VkCommandBuffer Command::AllocateCommandBuffer(bool primary){
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

void Command::BeginCommandBuffer(VkCommandBuffer cmdBuffer, VkCommandBufferUsageFlags usage){
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = usage;
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(cmdBuffer, &beginInfo) != VK_SUCCESS) {
        logger_.Log(LogLevel::EXCEPT, "Failed to begin recording command buffer!");
        throw std::runtime_error("Failed to begin recording command buffer!");
    }
}

void Command::EndCommandBuffer(VkCommandBuffer cmdBuffer){
    if (vkEndCommandBuffer(cmdBuffer) != VK_SUCCESS) {
        logger_.Log(LogLevel::EXCEPT, "Failed to record command buffer!");
        throw std::runtime_error("Failed to record command buffer!");
    }
}
void Command::Initialize(VkDevice device, QueueFamilyIndices indices, uint32_t swapchainImageCount) {
    device_ = device;

    // Create pool
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = indices.graphicsFamily.value();

    if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool_) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool!");
    }

    // Allocate one command buffer per swapchain framebuffer
    commandBuffers_.resize(swapchainImageCount);

    VkCommandBufferAllocateInfo alloc{};
    alloc.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc.commandPool = commandPool_;
    alloc.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc.commandBufferCount = swapchainImageCount;

    if (vkAllocateCommandBuffers(device, &alloc, commandBuffers_.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate per-frame command buffers");
    }

    logger_.Log(LogLevel::INFO, "Command initialized");
}

VkCommandBuffer Command::BeginFrame(uint32_t imageIndex) {
    VkCommandBuffer cmd = commandBuffers_[imageIndex];

    vkResetCommandBuffer(cmd, 0);

    VkCommandBufferBeginInfo begin{};
    begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(cmd, &begin) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin command buffer");
    }

    return cmd;
}

void Command::EndFrame(VkCommandBuffer cmd) {
    if (vkEndCommandBuffer(cmd) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer");
    }
}


void Command::Shutdown() {
    if (commandPool_ != VK_NULL_HANDLE) {
        vkDestroyCommandPool(device_, commandPool_, nullptr);
        commandPool_ = VK_NULL_HANDLE;
    }
    commandBuffers_.clear();

    logger_.Log(LogLevel::INFO, "Command destroyed");
}