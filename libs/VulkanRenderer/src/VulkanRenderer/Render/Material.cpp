#include "VulkanRenderer/Render/Material.hpp"

Material::Material(VkDevice device, VkPipeline pipeline, VkPipelineLayout layout)
    : device_(device), pipeline_(pipeline), pipelineLayout_(layout) {}

void Material::Bind(VkCommandBuffer cmd) {
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_);

    if (!descriptorSets_.empty()) {
        vkCmdBindDescriptorSets(cmd,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                pipelineLayout_,
                                0,
                                static_cast<uint32_t>(descriptorSets_.size()),
                                descriptorSets_.data(),
                                0,
                                nullptr);
    }
}
