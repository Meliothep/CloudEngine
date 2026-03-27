#include "Pipeline/PipelineBuilder.hpp"
#include <stdexcept>

void PipelineBuilder::Clear() {
    shaderStages_.clear();
    
    // 1. Vertex Input (Default Empty)
    vertexInputInfo_ = {};
    vertexInputInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    // 2. Input Assembly (Default Triangles)
    inputAssembly_ = {};
    inputAssembly_.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly_.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly_.primitiveRestartEnable = VK_FALSE;

    // 3. Rasterizer (Standard 3D Settings)
    rasterizer_ = {};
    rasterizer_.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer_.depthClampEnable = VK_FALSE;
    rasterizer_.rasterizerDiscardEnable = VK_FALSE;
    rasterizer_.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer_.lineWidth = 1.0f;
    rasterizer_.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer_.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer_.depthBiasEnable = VK_FALSE;

    // 4. Multisampling (Default 1 sample)
    multisampling_ = {};
    multisampling_.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling_.sampleShadingEnable = VK_FALSE;
    multisampling_.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // 5. Color Blending (Default: Alpha Blending Disabled)
    colorBlendAttachment_ = {};
    colorBlendAttachment_.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | 
                                           VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment_.blendEnable = VK_FALSE;

    colorBlending_ = {};
    colorBlending_.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending_.logicOpEnable = VK_FALSE;
    colorBlending_.attachmentCount = 1;
    colorBlending_.pAttachments = &colorBlendAttachment_;

    // 6. Viewport State (Dynamic Setup)
    viewportState_ = {};
    viewportState_.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState_.viewportCount = 1;
    viewportState_.scissorCount = 1;

    // 7. Dynamic States
    static const std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    dynamicState_ = {};
    dynamicState_.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState_.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState_.pDynamicStates = dynamicStates.data();

    // 8. Depth Stencil (Default Disabled)
    depthStencil_ = {};
    depthStencil_.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil_.depthTestEnable = VK_FALSE;
    depthStencil_.depthWriteEnable = VK_FALSE;
    depthStencil_.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil_.depthBoundsTestEnable = VK_FALSE;
    depthStencil_.stencilTestEnable = VK_FALSE;

    pipelineLayout_ = VK_NULL_HANDLE;
    renderPass_ = VK_NULL_HANDLE;
}

PipelineBuilder& PipelineBuilder::AddShaderStage(PipelineShaderStage shaderStage) {
    VkPipelineShaderStageCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.stage = shaderStage.stage;
    info.module = shaderStage.module;
    info.pName = "main"; // Hardcoded for simplicity as per your previous code
    
    shaderStages_.push_back(info);
    return *this;
}

PipelineBuilder& PipelineBuilder::SetVertexInput(uint32_t bindingCount, const VkVertexInputBindingDescription* bindings,
                                                uint32_t attrCount, const VkVertexInputAttributeDescription* attributes) {
    vertexInputInfo_.vertexBindingDescriptionCount = bindingCount;
    vertexInputInfo_.pVertexBindingDescriptions = bindings; 
    vertexInputInfo_.vertexAttributeDescriptionCount = attrCount;
    vertexInputInfo_.pVertexAttributeDescriptions = attributes;
    return *this;
}

PipelineBuilder& PipelineBuilder::SetInputAssembly(VkPrimitiveTopology topology, VkBool32 primitiveRestart) {
    inputAssembly_.topology = topology;
    inputAssembly_.primitiveRestartEnable = primitiveRestart;
    return *this;
}

PipelineBuilder& PipelineBuilder::SetRasterizer(VkPolygonMode polyMode, VkCullModeFlags cullMode, VkFrontFace frontFace) {
    rasterizer_.polygonMode = polyMode;
    rasterizer_.cullMode = cullMode;
    rasterizer_.frontFace = frontFace;
    return *this;
}

PipelineBuilder& PipelineBuilder::SetMultisampling(VkSampleCountFlagBits samples) {
    multisampling_.rasterizationSamples = samples;
    return *this;
}

PipelineBuilder& PipelineBuilder::SetColorBlend(VkBool32 enable, VkBlendFactor srcColor, VkBlendFactor dstColor) {
    colorBlendAttachment_.blendEnable = enable;
    colorBlendAttachment_.srcColorBlendFactor = srcColor;
    colorBlendAttachment_.dstColorBlendFactor = dstColor;
    colorBlendAttachment_.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment_.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment_.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment_.alphaBlendOp = VK_BLEND_OP_ADD;
    return *this;
}

PipelineBuilder& PipelineBuilder::SetRenderPass(VkRenderPass renderPass, uint32_t subpass) {
    renderPass_ = renderPass;
    subpass_ = subpass;
    return *this;
}

PipelineBuilder& PipelineBuilder::SetPipelineLayout(VkPipelineLayout layout) {
    pipelineLayout_ = layout;
    return *this;
}

std::unique_ptr<Pipeline> PipelineBuilder::Build() {
    if (renderPass_ == VK_NULL_HANDLE) throw std::runtime_error("PipelineBuilder: RenderPass not set!");
    if (pipelineLayout_ == VK_NULL_HANDLE) throw std::runtime_error("PipelineBuilder: PipelineLayout not set!");
    if (shaderStages_.empty()) throw std::runtime_error("PipelineBuilder: No shader stages added!");

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages_.size());
    pipelineInfo.pStages = shaderStages_.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo_;
    pipelineInfo.pInputAssemblyState = &inputAssembly_;
    pipelineInfo.pViewportState = &viewportState_;
    pipelineInfo.pRasterizationState = &rasterizer_;
    pipelineInfo.pMultisampleState = &multisampling_;
    pipelineInfo.pDepthStencilState = &depthStencil_;
    pipelineInfo.pColorBlendState = &colorBlending_;
    pipelineInfo.pDynamicState = &dynamicState_;
    pipelineInfo.layout = pipelineLayout_;
    pipelineInfo.renderPass = renderPass_;
    pipelineInfo.subpass = subpass_;

    auto pipeline = std::make_unique<Pipeline>(logger_);
    
    // Note: You need to add this method to your Pipeline class
    pipeline->Initialize(device_, pipelineInfo);

    return pipeline;
}