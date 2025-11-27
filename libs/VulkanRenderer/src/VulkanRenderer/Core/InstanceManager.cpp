#include "VulkanRenderer/Core/InstanceManager.hpp"

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    Logger* logger = reinterpret_cast<Logger*>(pUserData);

    std::string severity;
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) severity = "VERBOSE";
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) severity = "INFO";
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) severity = "WARNING";
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) severity = "ERROR";

    logger->Log(LogLevel::DEBUG, std::string("[Vulkan ") + severity + std::string("]: ") + (pCallbackData->pMessage ? pCallbackData->pMessage : ""));

    return VK_FALSE; // VK_FALSE = don't abort Vulkan call
}

void InstanceManager::Initialize(bool enableValidationLayers, bool enableScreen){

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "CloudEngine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "CloudEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = GetRequiredExtensions(enableValidationLayers, enableScreen);
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
        if (!CheckValidationLayerSupport()) {
            logger_.Log(LogLevel::EXCEPT, "Validation layers requested but not available!");
            throw std::runtime_error("Validation layers missing");
        }

        const char* validationLayers[] = { "VK_LAYER_KHRONOS_validation" };
        createInfo.enabledLayerCount = 1;
        createInfo.ppEnabledLayerNames = validationLayers;

        // Optional: populate debug messenger info for instance creation
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        
        debugCreateInfo.pUserData = &logger_;
        debugCreateInfo.pfnUserCallback = DebugCallback;
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
        createInfo.pNext = nullptr;
    }

    VkResult res = vkCreateInstance(&createInfo, nullptr, &instance_);
    if (res != VK_SUCCESS) {
        logger_.Log(LogLevel::CRITICAL, std::string("Failed to create Vulkan instance, VkResult=") + std::to_string(static_cast<int>(res)));
        throw std::runtime_error("Failed to create Vulkan instance");
    }

    logger_.Log(LogLevel::INFO, "Vulkan instance created successfully");

    // If validation layers were requested, create an explicit debug messenger
    if (enableValidationLayers) {
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pUserData = &logger_;
        debugCreateInfo.pfnUserCallback = DebugCallback;

        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance_, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            VkResult r = func(instance_, &debugCreateInfo, nullptr, &debugMessenger_);
            if (r != VK_SUCCESS) {
                logger_.Log(LogLevel::EXCEPT, std::string("vkCreateDebugUtilsMessengerEXT returned ") + std::to_string(static_cast<int>(r)));
            } else {
                logger_.Log(LogLevel::INFO, "Debug utils messenger created successfully");
            }
        } else {
            logger_.Log(LogLevel::EXCEPT, "vkCreateDebugUtilsMessengerEXT not found; validation messages may be limited");
        }
        validationEnabled_ = true;
    }
}

void InstanceManager::Shutdown() {
    if (debugMessenger_ != VK_NULL_HANDLE) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance_, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance_, debugMessenger_, nullptr);
        }
    }

    if (instance_ != VK_NULL_HANDLE) {
        vkDestroyInstance(instance_, nullptr);
        instance_ = VK_NULL_HANDLE;
    }

    logger_.Log(LogLevel::INFO, "Vulkan instance destroyed successfully");
}

bool InstanceManager::CheckValidationLayerSupport() {
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    const char* validationLayerName = "VK_LAYER_KHRONOS_validation";
    for (const auto& layer : availableLayers) {
        if (strcmp(layer.layerName, validationLayerName) == 0) {
            return true;
        }
    }

    return false;
}

std::vector<const char*> InstanceManager::GetRequiredExtensions(bool enableValidationLayers, bool enableScreen) {
    std::vector<const char*> extensions;

    // Debug extension
    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    // Screen/windowed rendering extensions
    if (enableScreen) {
        extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

        #ifdef _WIN32
            extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
        #elif defined(__linux__)
            extensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
        #elif defined(__APPLE__)
            extensions.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
        #else
            #error "Platform not supported"
        #endif
    }

    std::string msg = std::string("Required extensions for this instance: ") + std::to_string(extensions.size());
    logger_.Log(LogLevel::DEBUG, msg);
    return extensions;
}
