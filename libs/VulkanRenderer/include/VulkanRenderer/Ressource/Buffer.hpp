#pragma once
#include "pch.h"

class Buffer {
public:
    Buffer() = default;
    ~Buffer() = default;

    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkDeviceSize size = 0;

    void Create(VkDevice device,
                VkPhysicalDevice physicalDevice,
                VkDeviceSize size,
                VkBufferUsageFlags usage,
                VkMemoryPropertyFlags properties);

    void Destroy();

    void CopyToBuffer(const void* data, VkDeviceSize dataSize);

    static void Copy(VkDevice device,
                     VkQueue queue,
                     VkCommandPool commandPool,
                     VkBuffer src,
                     VkBuffer dst,
                     VkDeviceSize size);

private:
    VkDevice device_ = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;

    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};
