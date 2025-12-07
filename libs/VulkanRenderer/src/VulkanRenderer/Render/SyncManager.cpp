#include "VulkanRenderer/Render/SyncManager.hpp"

void SyncManager::Initialize(VkDevice device, uint32_t frameCount){
    device_ = device;
    FRAME_COUNT = frameCount;

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    
    imageAvailableSemaphores_.resize(FRAME_COUNT);
    renderFinishedSemaphores_.resize(FRAME_COUNT);
    inFlightFences_.resize(FRAME_COUNT);

    for (int i = 0; i < FRAME_COUNT; i++) {
        if (vkCreateSemaphore(device_, &semaphoreInfo, nullptr, &imageAvailableSemaphores_[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device_, &semaphoreInfo, nullptr, &renderFinishedSemaphores_[i]) != VK_SUCCESS ||
            vkCreateFence(device_, &fenceInfo, nullptr, &inFlightFences_[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create semaphores!");
        }
    }

    logger_.Log(LogLevel::INFO, "Sync objects created successfully");
}

void SyncManager::WaitForFence(){
    vkWaitForFences(device_, 1, &inFlightFences_[currentFrame], VK_TRUE, UINT64_MAX);
}

void SyncManager::AdvanceFrame(){
    currentFrame = (currentFrame + 1) % FRAME_COUNT;
    vkResetFences(device_, 1, &inFlightFences_[currentFrame]);  
}

void SyncManager::Shutdown(){
    
    for (int i = 0; i < FRAME_COUNT; i++) {
        vkDestroySemaphore(device_, imageAvailableSemaphores_[i], nullptr);
        imageAvailableSemaphores_[i] = VK_NULL_HANDLE;

        vkDestroySemaphore(device_, renderFinishedSemaphores_[i], nullptr);
        renderFinishedSemaphores_[i] = VK_NULL_HANDLE;

        vkDestroyFence(device_, inFlightFences_[i], nullptr);
        inFlightFences_[i] = VK_NULL_HANDLE;
    }

    logger_.Log(LogLevel::INFO, "Sync objects destroyed successfully");
}