#pragma once
#include "pch.h"


class Material {
public:
    Material(VkDevice device, VkPipeline pipeline, VkPipelineLayout layout);
    ~Material() = default;

    void Bind(VkCommandBuffer cmd);

    VkPipelineLayout GetPipelineLayout() const { return pipelineLayout_; }
    VkPipeline GetPipeline() const { return pipeline_; }

    void SetDescriptorSets(const std::vector<VkDescriptorSet>& sets) { descriptorSets_ = sets; }

private:
    VkDevice device_{ VK_NULL_HANDLE };
    VkPipeline pipeline_{ VK_NULL_HANDLE };
    VkPipelineLayout pipelineLayout_{ VK_NULL_HANDLE };
    std::vector<VkDescriptorSet> descriptorSets_;
};

