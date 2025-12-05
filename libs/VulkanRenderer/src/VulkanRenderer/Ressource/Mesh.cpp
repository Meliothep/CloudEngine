#include "VulkanRenderer/Ressource/Mesh.hpp"

void Mesh::Initialize(VkDevice device,
                      VkPhysicalDevice physicalDevice,
                      VkQueue graphicsQueue,
                      VkCommandPool commandPool,
                      const std::vector<Vertex>& vertices,
                      const std::vector<uint32_t>& indices)
{
    device_ = device;
    physicalDevice_ = physicalDevice;
    graphicsQueue_ = graphicsQueue;
    commandPool_ = commandPool;

    vertexCount_ = static_cast<uint32_t>(vertices.size());
    indexCount_ = static_cast<uint32_t>(indices.size());
    hasIndices_ = !indices.empty();

    CreateVertexBuffer(vertices);

    if (hasIndices_) {
        CreateIndexBuffer(indices);
    }

    logger_.Log(LogLevel::INFO, "Mesh initialized: " 
        + std::to_string(vertexCount_) + " vertices, "
        + std::to_string(indexCount_) + " indices");
}

void Mesh::Shutdown() {
    vertexBuffer_.Destroy();
    indexBuffer_.Destroy();
}

void Mesh::Bind(VkCommandBuffer cmd) {
    VkBuffer vertexBuffers[] = { vertexBuffer_.buffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(cmd, 0, 1, vertexBuffers, offsets);

    if (hasIndices_) {
        vkCmdBindIndexBuffer(cmd, indexBuffer_.buffer, 0, VK_INDEX_TYPE_UINT32);
    }
}

void Mesh::CreateVertexBuffer(const std::vector<Vertex>& vertices) {
    VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();

    Buffer staging;
    staging.Create(device_, physicalDevice_, bufferSize,
                   VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    staging.CopyToBuffer(vertices.data(), bufferSize);

    vertexBuffer_.Create(device_, physicalDevice_, bufferSize,
                         VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    CopyBuffer(staging.buffer, vertexBuffer_.buffer, bufferSize);

    staging.Destroy();
}

void Mesh::CreateIndexBuffer(const std::vector<uint32_t>& indices) {
    VkDeviceSize bufferSize = sizeof(uint32_t) * indices.size();

    Buffer staging;
    staging.Create(device_, physicalDevice_, bufferSize,
                   VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    staging.CopyToBuffer(indices.data(), bufferSize);

    indexBuffer_.Create(device_, physicalDevice_, bufferSize,
                        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    CopyBuffer(staging.buffer, indexBuffer_.buffer, bufferSize);

    staging.Destroy();
}

void Mesh::CopyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size) {
    Buffer::Copy(device_, graphicsQueue_, commandPool_, src, dst, size);
}

