#pragma once
#include "pch.h"

#include "Pipeline.hpp"

class PipelineBuilder {
public:
    PipelineBuilder(VkDevice device, Logger& logger) 
        : device_(device), logger_(logger) {}

    // Required — must call before Build()
    PipelineBuilder& AddShaderStage(PipelineShaderStage shaderStage);
    PipelineBuilder& SetVertexInput(uint32_t bindingCount, const VkVertexInputBindingDescription* bindings,
                                    uint32_t attrCount, const VkVertexInputAttributeDescription* attributes);
    PipelineBuilder& SetInputAssembly(VkPrimitiveTopology topology, VkBool32 primitiveRestart = VK_FALSE);
    PipelineBuilder& SetRasterizer(VkPolygonMode polyMode, VkCullModeFlags cullMode, VkFrontFace frontFace);
    PipelineBuilder& SetMultisampling(VkSampleCountFlagBits samples);
    PipelineBuilder& SetColorBlend(VkBool32 enable, VkBlendFactor srcColor = VK_BLEND_FACTOR_ONE,
                                   VkBlendFactor dstColor = VK_BLEND_FACTOR_ZERO);
    PipelineBuilder& SetDepthStencil(VkBool32 depthTest, VkBool32 depthWrite, VkCompareOp compareOp);
    PipelineBuilder& SetViewportState(uint32_t viewportCount = 1, uint32_t scissorCount = 1);
    PipelineBuilder& SetDynamicStates(const std::vector<VkDynamicState>& dynamicStates);
    PipelineBuilder& SetRenderPass(VkRenderPass renderPass, uint32_t subpass = 0);
    PipelineBuilder& SetPipelineLayout(uint32_t setLayoutCount, const VkDescriptorSetLayout* pSetLayouts,
                                   uint32_t pushConstantRangeCount, const VkPushConstantRange* pPushConstantRanges);

    // Builds the pipeline — throws if any required state is missing
    std::unique_ptr<Pipeline> Build();

private:
    VkDevice device_;
    Logger& logger_;

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages_;

    VkPipelineVertexInputStateCreateInfo   vertexInputInfo_{};
    VkPipelineInputAssemblyStateCreateInfo inputAssembly_{};
    VkPipelineViewportStateCreateInfo      viewportState_{};
    VkPipelineRasterizationStateCreateInfo rasterizer_{};
    VkPipelineMultisampleStateCreateInfo   multisampling_{};
    VkPipelineColorBlendAttachmentState    colorBlendAttachment_{};
    VkPipelineColorBlendStateCreateInfo    colorBlending_{};
    VkPipelineDepthStencilStateCreateInfo  depthStencil_{};
    VkPipelineDynamicStateCreateInfo       dynamicState_{};
    std::vector<VkDynamicState>            dynamicStatesStorage_;

    VkPipelineLayout pipelineLayout_ = VK_NULL_HANDLE;
    VkRenderPass     renderPass_     = VK_NULL_HANDLE;
    uint32_t         subpass_        = 0;

    // Tracks which states have been explicitly configured
    bool hasShaderStages_   = false;
    bool hasVertexInput_    = false;
    bool hasInputAssembly_  = false;
    bool hasRasterizer_     = false;
    bool hasMultisampling_  = false;
    bool hasColorBlend_     = false;
    bool hasDepthStencil_   = false;
    bool hasViewportState_  = false;
    bool hasDynamicStates_  = false;
    bool hasRenderPass_     = false;
    bool hasPipelineLayout_ = false;
};