#pragma once
#include "pch.h"

class SyncManager {
public:
    SyncManager(Logger& logger) : logger_(logger) {}
    ~SyncManager() = default;

    uint32_t GetCurrentFrame() const { return currentFrame;}
    VkSemaphore GetImageAvailableSemaphore() const { return imageAvailableSemaphores_[currentFrame]; }
    VkSemaphore GetRenderFinishedSemaphore() const { return renderFinishedSemaphores_[currentFrame]; }
    VkFence GetFence() const { return inFlightFences_[currentFrame]; }

    void Initialize(VkDevice device, uint32_t frameCount);
    void Shutdown();

    void WaitForFence();
    void AdvanceFrame();
    
private:
    Logger& logger_;
    VkDevice device_{ VK_NULL_HANDLE };

    uint32_t currentFrame = 0;
    uint32_t FRAME_COUNT = 0;
    
    std::vector<VkSemaphore> imageAvailableSemaphores_{ VK_NULL_HANDLE };
    std::vector<VkSemaphore> renderFinishedSemaphores_{ VK_NULL_HANDLE };
    std::vector<VkFence> inFlightFences_{ VK_NULL_HANDLE };
};
