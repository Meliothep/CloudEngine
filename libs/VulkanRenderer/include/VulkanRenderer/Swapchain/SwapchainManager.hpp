#pragma once
#include "pch.h"

#include "Windows/IWindow.hpp" 
#include "VulkanRenderer/Ressource/QueueFamilyIndices.hpp"

struct SwapchainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class SwapchainManager {
public:
    SwapchainManager(Logger& logger) : logger_(logger) {}
    ~SwapchainManager() = default;

    void Initialize(VkPhysicalDevice physicalDevice,
                    VkDevice device,
                    VkSurfaceKHR surface,
                    QueueFamilyIndices indices,
                    IWindow* window);

    void Shutdown();

    VkSwapchainKHR GetSwapchain() const { return swapchain_; }
    VkFormat GetImageFormat() const { return imageFormat_; }
    VkExtent2D GetExtent() const { return extent_; }
    const std::vector<VkImageView>& GetImageViews() const { return imageViews_; }

private:
    Logger& logger_;

    VkDevice device_{VK_NULL_HANDLE};
    VkSwapchainKHR swapchain_{VK_NULL_HANDLE};
    std::vector<VkImage> images_;
    std::vector<VkImageView> imageViews_;
    VkFormat imageFormat_{};
    VkExtent2D extent_{};

    // Helpers
    SwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);

    void CreateImageViews();
};
