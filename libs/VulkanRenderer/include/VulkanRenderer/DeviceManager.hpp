#pragma once
#include "Utils/Logger.hpp"

#include <vulkan/vulkan.h>
#include <optional>
#include <vector>
#include <set>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    std::optional<uint32_t> computeFamily;
    std::optional<uint32_t> transferFamily;

    bool IsComplete(bool enableScreen) const {
        if (enableScreen) {
            return graphicsFamily.has_value() && presentFamily.has_value();
        } else {
            return graphicsFamily.has_value();
        }
    }
};

class DeviceManager {
public:
    DeviceManager(Logger& logger) : logger_(logger) {}
    ~DeviceManager() { Shutdown(); };

    void Initialize(VkInstance instance, VkSurfaceKHR surface = VK_NULL_HANDLE, bool enableScreen = false);

    void Shutdown();

    VkDevice GetDevice() const { return device_; }
    VkPhysicalDevice GetPhysicalDevice() const { return physicalDevice_; }
    VkQueue GetGraphicsQueue() const { return graphicsQueue_; }
    VkQueue GetComputeQueue() const { return computeQueue_; }
    VkQueue GetPresentQueue() const { return presentQueue_; }

    const QueueFamilyIndices& GetQueueFamilyIndices() const { return queueFamilyIndices_; }

private:
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface, bool enableScreen);
    int RateDeviceSuitability(VkPhysicalDevice device);

    void ConfigureExtentions(bool enableScreen);

    std::vector<const char*> GetRequiredExtensions(bool enableValidationLayers, bool enableSurface);
    void GetPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, bool enableScreen);
    void GetLogicalDevice(QueueFamilyIndices indices, bool enableScreen);
    void GetQueue(QueueFamilyIndices indices, bool enableScreen);
    
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
private:
    Logger& logger_;

    VkPhysicalDevice physicalDevice_{ VK_NULL_HANDLE };
    VkDevice device_{ VK_NULL_HANDLE };

    VkQueue graphicsQueue_{ VK_NULL_HANDLE };
    VkQueue computeQueue_{ VK_NULL_HANDLE };
    VkQueue presentQueue_{ VK_NULL_HANDLE };

    std::vector<const char*> requiredDeviceExtensions_;
    QueueFamilyIndices queueFamilyIndices_;
};
