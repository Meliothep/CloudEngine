#pragma once
#include "pch.h"

#include "Pipeline.hpp"

class PipelineBuilder {
public:
    PipelineBuilder(VkDevice device, Logger& logger) 
        : device_(device), logger_(logger) {
        Clear();
    }

    void Clear();

    // Configuration Methods
    PipelineBuilder& AddShaderStage(PipelineShaderStage shaderStage);
    PipelineBuilder& SetVertexInput(uint32_t bindingCount, const VkVertexInputBindingDescription* bindings,
                                    uint32_t attrCount, const VkVertexInputAttributeDescription* attributes);
    PipelineBuilder& SetInputAssembly(VkPrimitiveTopology topology, VkBool32 primitiveRestart = VK_FALSE);
    PipelineBuilder& SetRasterizer(VkPolygonMode polyMode, VkCullModeFlags cullMode, VkFrontFace frontFace);
    PipelineBuilder& SetMultisampling(VkSampleCountFlagBits samples);
    PipelineBuilder& SetColorBlend(VkBool32 enable, VkBlendFactor srcColor = VK_BLEND_FACTOR_ONE, 
                                   VkBlendFactor dstColor = VK_BLEND_FACTOR_ZERO);
    PipelineBuilder& SetRenderPass(VkRenderPass renderPass, uint32_t subpass = 0);
    PipelineBuilder& SetPipelineLayout(VkPipelineLayout layout);

    // The Final Build
    std::unique_ptr<Pipeline> Build();

private:
    VkDevice device_;
    Logger& logger_;

    // Temporary storage for pipeline state
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages_;
    VkPipelineVertexInputStateCreateInfo vertexInputInfo_{};
    VkPipelineInputAssemblyStateCreateInfo inputAssembly_{};
    VkPipelineViewportStateCreateInfo viewportState_{};
    VkPipelineRasterizationStateCreateInfo rasterizer_{};
    VkPipelineMultisampleStateCreateInfo multisampling_{};
    VkPipelineColorBlendAttachmentState colorBlendAttachment_{};
    VkPipelineColorBlendStateCreateInfo colorBlending_{};
    VkPipelineDepthStencilStateCreateInfo depthStencil_{};
    VkPipelineDynamicStateCreateInfo dynamicState_{};
    
    VkPipelineLayout pipelineLayout_ = VK_NULL_HANDLE;
    VkRenderPass renderPass_ = VK_NULL_HANDLE;
    uint32_t subpass_ = 0;
};