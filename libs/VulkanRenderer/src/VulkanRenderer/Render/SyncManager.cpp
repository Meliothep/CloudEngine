#include "VulkanRenderer/Render/SyncManager.hpp"

void SyncManager::Initialize(VkDevice device){
    device_ = device;

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(device_, &semaphoreInfo, nullptr, &imageAvailableSemaphore_) != VK_SUCCESS ||
        vkCreateSemaphore(device_, &semaphoreInfo, nullptr, &renderFinishedSemaphore_) != VK_SUCCESS ||
        vkCreateFence(device_, &fenceInfo, nullptr, &inFlightFence_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create semaphores!");
    }

    logger_.Log(LogLevel::INFO, "Sync objects created successfully");
}

void SyncManager::WaitForFence(){
    vkWaitForFences(device_, 1, &inFlightFence_, VK_TRUE, UINT64_MAX);
    vkResetFences(device_, 1, &inFlightFence_);    
}

void SyncManager::Shutdown(){
    vkDestroySemaphore(device_, imageAvailableSemaphore_, nullptr);
    imageAvailableSemaphore_ = VK_NULL_HANDLE;

    vkDestroySemaphore(device_, renderFinishedSemaphore_, nullptr);
    renderFinishedSemaphore_ = VK_NULL_HANDLE;

    vkDestroyFence(device_, inFlightFence_, nullptr);
    inFlightFence_ = VK_NULL_HANDLE;

    logger_.Log(LogLevel::INFO, "Sync objects destroyed successfully");
}