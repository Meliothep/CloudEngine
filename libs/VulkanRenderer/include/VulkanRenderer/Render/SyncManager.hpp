#pragma once
#include "Utils/Logger.hpp"

#include <vulkan/vulkan.h>

class SyncManager {
public:
    SyncManager(Logger& logger) : logger_(logger) {}
    ~SyncManager() = default;

    VkSemaphore GetImageAvailableSemaphore() const { return imageAvailableSemaphore_; }
    VkSemaphore GetRenderFinishedSemaphore() const { return renderFinishedSemaphore_; }
    VkFence GetFence() const { return inFlightFence_; }

    void Initialize(VkDevice device);
    void Shutdown();

    void WaitForFence();
    
private:
    Logger& logger_;
    VkDevice device_{ VK_NULL_HANDLE };

    VkSemaphore imageAvailableSemaphore_{ VK_NULL_HANDLE };
    VkSemaphore renderFinishedSemaphore_{ VK_NULL_HANDLE };
    VkFence inFlightFence_{ VK_NULL_HANDLE };
};
