#include "VulkanRenderer/Core/DeviceManager.hpp"
#include <map>
int DeviceManager::RateDeviceSuitability(VkPhysicalDevice device) {
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    
    int score = 0;
    
    // Log device info for debugging
    logger_.Log(LogLevel::DEBUG, std::format("Evaluating device: {}", deviceProperties.deviceName));
    logger_.Log(LogLevel::DEBUG, std::format("  Device type: {}", static_cast<int>(deviceProperties.deviceType)));
    logger_.Log(LogLevel::DEBUG, std::format("  API version: {}.{}.{}", 
        VK_VERSION_MAJOR(deviceProperties.apiVersion),
        VK_VERSION_MINOR(deviceProperties.apiVersion),
        VK_VERSION_PATCH(deviceProperties.apiVersion)));
    
    // CRITICAL: Heavily penalize software/CPU renderers
    // Check for "Basic Render Driver" or CPU device type
    std::string deviceName = deviceProperties.deviceName;
    if (deviceName.find("Basic Render") != std::string::npos || 
        deviceName.find("Software") != std::string::npos ||
        deviceName.find("llvmpipe") != std::string::npos ||
        deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU) {
        logger_.Log(LogLevel::DEBUG, std::format("Device '{}' is a software renderer - heavily penalized", deviceName));
        score -= 100000;  // Massive penalty to avoid software renderers
    }
    
    // Needs to support extensions
    if (!CheckDeviceExtensionSupport(device)) {
        logger_.Log(LogLevel::WARN, "  Device doesn't support required extensions");
        return 0;  // Disqualify entirely
    }
    
    // Prioritize device types
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 10000;
        logger_.Log(LogLevel::DEBUG, "  Discrete GPU: +10000");
    } else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
        score += 5000;
        logger_.Log(LogLevel::DEBUG, "  Integrated GPU: +5000");
    } else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU) {
        score += 1000;
        logger_.Log(LogLevel::DEBUG, "  Virtual GPU: +1000");
    }
    
    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D / 10;  // Scaled down to not overwhelm device type score
    
    // Application can't function without geometry shaders (if you really need them)
    // Note: Many modern apps don't actually need geometry shaders
    if (!deviceFeatures.geometryShader) {
        logger_.Log(LogLevel::WARN, "  Device doesn't support geometry shaders");
        return 0;  // Disqualify if you really need geometry shaders
        // OR just penalize: score -= 1000;
    }
    
    logger_.Log(LogLevel::DEBUG, std::format("  Final score: {}", score));
    
    return score;
}

void DeviceManager::ConfigureExtentions(bool enableScreen){
    // Extentions 
    requiredDeviceExtensions_.clear();
    if (enableScreen) {
        requiredDeviceExtensions_.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }
}

bool DeviceManager::CheckDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> required(requiredDeviceExtensions_.begin(), requiredDeviceExtensions_.end());

    for (const auto& ext : availableExtensions) {
        required.erase(ext.extensionName);
    }

    return required.empty();
}

void DeviceManager::GetPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, bool enableScreen){
    // Physical device 
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        logger_.Log(LogLevel::EXCEPT, "Failed to find GPUs with Vulkan support!");
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    // Use an ordered map to automatically sort candidates by increasing score
    std::multimap<int, VkPhysicalDevice> candidates;

    for (const auto& device : devices) {
        int score = RateDeviceSuitability(device);
        candidates.insert(std::make_pair(score, device));
    }

    // Check if the best candidate is suitable at all
    if (candidates.rbegin()->first > 0) {
        physicalDevice_ = candidates.rbegin()->second;
    } else {
        logger_.Log(LogLevel::EXCEPT, "Failed to find a suitable GPU!");
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

void DeviceManager::GetLogicalDevice(QueueFamilyIndices indices, bool enableScreen){
    // --- Logical device creation
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {
        indices.graphicsFamily.value(),
        indices.computeFamily.value_or(indices.graphicsFamily.value())
    };

    if (enableScreen && indices.presentFamily.has_value()) {
        uniqueQueueFamilies.insert(indices.presentFamily.value());
    }

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{}; // can enable features later

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions_.size());
    createInfo.ppEnabledExtensionNames = requiredDeviceExtensions_.data();

    if (vkCreateDevice(physicalDevice_, &createInfo, nullptr, &device_) != VK_SUCCESS) {
        logger_.Log(LogLevel::CRITICAL, "Failed to create logical device!");
        throw std::runtime_error("failed to create logical device!");
    }
}

void DeviceManager::GetQueue(QueueFamilyIndices indices, bool enableScreen){
    // --- Get queues
    vkGetDeviceQueue(device_, indices.graphicsFamily.value(), 0, &graphicsQueue_);
    
    if (indices.computeFamily.has_value()) {
        vkGetDeviceQueue(device_, indices.computeFamily.value(), 0, &computeQueue_);
    } else {
        computeQueue_ = graphicsQueue_; // fallback
    }

    if (enableScreen && indices.presentFamily.has_value()) {
        vkGetDeviceQueue(device_, indices.presentFamily.value(), 0, &presentQueue_);
    }

    logger_.Log(LogLevel::INFO, "Logical device created successfully");
}

void DeviceManager::Initialize(VkInstance instance, VkSurfaceKHR surface, bool enableScreen){
    ConfigureExtentions(enableScreen);
    GetPhysicalDevice(instance, surface, enableScreen);

    // --- Queue families
    queueFamilyIndices_ = FindQueueFamilies(physicalDevice_, surface, enableScreen);
    if (!queueFamilyIndices_.IsComplete(enableScreen)) {
        logger_.Log(LogLevel::CRITICAL, "Selected GPU does not support required queue families!");
        throw std::runtime_error("Selected GPU does not support required queue families!");
    }

    GetLogicalDevice(queueFamilyIndices_, enableScreen);
    GetQueue(queueFamilyIndices_, enableScreen);

    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(physicalDevice_, &props);
    logger_.Log(LogLevel::INFO, std::string("Using GPU: ") + props.deviceName + std::string(" (Driver version ") +
        std::to_string(VK_VERSION_MAJOR(props.driverVersion)) + std::string(".") +
        std::to_string(VK_VERSION_MINOR(props.driverVersion)) + std::string(".") +
        std::to_string(VK_VERSION_PATCH(props.driverVersion)) + std::string(")")
    );
}

QueueFamilyIndices DeviceManager::FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface, bool enableScreen) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        if (enableScreen) {
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
            if (presentSupport) {
                indices.presentFamily = i;
            }
        }

        if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
            indices.computeFamily = i;
        }

        if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) {
            indices.transferFamily = i;
        }

        if ((queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) && 
            !(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
            !(queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)) {
            indices.transferFamily = i;
        }

        if (indices.IsComplete(enableScreen)) break;
        i++;
    }

    return indices;
}

void DeviceManager::Shutdown() {
    if (device_ != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(device_);
        vkDestroyDevice(device_, nullptr);
        device_ = VK_NULL_HANDLE;
        graphicsQueue_ = VK_NULL_HANDLE;
        computeQueue_  = VK_NULL_HANDLE;
        presentQueue_  = VK_NULL_HANDLE;
        queueFamilyIndices_;
        logger_.Log(LogLevel::INFO, "Logical device destroyed successfully");
    }
}