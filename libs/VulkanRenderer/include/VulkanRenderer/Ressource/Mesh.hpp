#pragma once
#include "pch.h"

#include "Buffer.hpp"
#include "Vertex.hpp"

class Mesh {
public:
    Mesh(Logger& logger) : logger_(logger) {}
    ~Mesh() = default;


    void Initialize(VkDevice device,
                    VkPhysicalDevice physicalDevice,
                    VkQueue graphicsQueue,
                    VkCommandPool commandPool,
                    const std::vector<Vertex>& vertices,
                    const std::vector<uint32_t>& indices = {});

    void Shutdown();

    void Bind(VkCommandBuffer cmd);

    bool HasIndices() const { return hasIndices_; }
    uint32_t GetIndexCount() const { return indexCount_; }
    uint32_t GetVertexCount() const { return vertexCount_; }
    VkIndexType GetIndexType() const { return VK_INDEX_TYPE_UINT32; }

private:
    Logger& logger_;

    VkDevice device_{ VK_NULL_HANDLE };
    VkPhysicalDevice physicalDevice_{ VK_NULL_HANDLE };
    VkQueue graphicsQueue_{ VK_NULL_HANDLE };
    VkCommandPool commandPool_{ VK_NULL_HANDLE };

    Buffer vertexBuffer_;
    Buffer indexBuffer_;

    uint32_t vertexCount_ = 0;
    uint32_t indexCount_ = 0;
    bool hasIndices_ = false;

    void CreateVertexBuffer(const std::vector<Vertex>& vertices);
    void CreateIndexBuffer(const std::vector<uint32_t>& indices);

    void CopyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);
};

