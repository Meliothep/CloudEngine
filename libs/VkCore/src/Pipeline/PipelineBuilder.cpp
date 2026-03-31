#include "Pipeline/PipelineBuilder.hpp"
#include "Pipeline/PipelineUtils.hpp"

// ─────────────────────────────────────────────────────────────────────────────
// Shader Stages
// ─────────────────────────────────────────────────────────────────────────────

PipelineBuilder& PipelineBuilder::AddShaderStage(PipelineShaderStage shaderStage) {
    VkPipelineShaderStageCreateInfo info{};
    info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.stage  = shaderStage.stage;
    info.module = shaderStage.module;
    info.pName  = "main";

    shaderStages_.push_back(info);
    hasShaderStages_ = true;
    logger_.Log(LogLevel::DEBUG, "Added shader stage to builder: stage=" + std::to_string(shaderStage.stage));
    return *this;
}

// ─────────────────────────────────────────────────────────────────────────────
// Vertex Input
// ─────────────────────────────────────────────────────────────────────────────

PipelineBuilder& PipelineBuilder::SetVertexInput(uint32_t bindingCount, const VkVertexInputBindingDescription* bindings,
                                                  uint32_t attrCount,    const VkVertexInputAttributeDescription* attributes) {
    vertexInputInfo_ = {};
    vertexInputInfo_.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo_.vertexBindingDescriptionCount   = bindingCount;
    vertexInputInfo_.pVertexBindingDescriptions      = bindings;
    vertexInputInfo_.vertexAttributeDescriptionCount = attrCount;
    vertexInputInfo_.pVertexAttributeDescriptions    = attributes;

    hasVertexInput_ = true;
    logger_.Log(LogLevel::DEBUG, "Vertex input state set in builder");
    return *this;
}

// ─────────────────────────────────────────────────────────────────────────────
// Input Assembly
// ─────────────────────────────────────────────────────────────────────────────

PipelineBuilder& PipelineBuilder::SetInputAssembly(VkPrimitiveTopology topology, VkBool32 primitiveRestart) {
    inputAssembly_ = {};
    inputAssembly_.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly_.topology               = topology;
    inputAssembly_.primitiveRestartEnable = primitiveRestart;

    hasInputAssembly_ = true;
    logger_.Log(LogLevel::DEBUG, "Input assembly state set in builder");
    return *this;
}

// ─────────────────────────────────────────────────────────────────────────────
// Rasterizer
// ─────────────────────────────────────────────────────────────────────────────

PipelineBuilder& PipelineBuilder::SetRasterizer(VkPolygonMode polyMode, VkCullModeFlags cullMode, VkFrontFace frontFace) {
    rasterizer_ = {};
    rasterizer_.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer_.depthClampEnable        = VK_FALSE;
    rasterizer_.rasterizerDiscardEnable = VK_FALSE;
    rasterizer_.lineWidth               = 1.0f;
    rasterizer_.depthBiasEnable         = VK_FALSE;
    rasterizer_.polygonMode             = polyMode;
    rasterizer_.cullMode                = cullMode;
    rasterizer_.frontFace               = frontFace;

    hasRasterizer_ = true;
    logger_.Log(LogLevel::DEBUG, "Rasterizer state set in builder");
    return *this;
}

// ─────────────────────────────────────────────────────────────────────────────
// Multisampling
// ─────────────────────────────────────────────────────────────────────────────

PipelineBuilder& PipelineBuilder::SetMultisampling(VkSampleCountFlagBits samples) {
    multisampling_ = {};
    multisampling_.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling_.sampleShadingEnable  = VK_FALSE;
    multisampling_.rasterizationSamples = samples;

    hasMultisampling_ = true;
    logger_.Log(LogLevel::DEBUG, "Multisampling state set in builder");
    return *this;
}

// ─────────────────────────────────────────────────────────────────────────────
// Color Blend
// ─────────────────────────────────────────────────────────────────────────────

PipelineBuilder& PipelineBuilder::SetColorBlend(VkBool32 enable, VkBlendFactor srcColor, VkBlendFactor dstColor) {
    colorBlendAttachment_ = {};
    colorBlendAttachment_.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                                VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment_.blendEnable         = enable;
    colorBlendAttachment_.srcColorBlendFactor = srcColor;
    colorBlendAttachment_.dstColorBlendFactor = dstColor;
    colorBlendAttachment_.colorBlendOp        = VK_BLEND_OP_ADD;
    colorBlendAttachment_.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment_.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment_.alphaBlendOp        = VK_BLEND_OP_ADD;

    colorBlending_ = {};
    colorBlending_.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending_.logicOpEnable   = VK_FALSE;
    colorBlending_.attachmentCount = 1;
    colorBlending_.pAttachments    = &colorBlendAttachment_;

    hasColorBlend_ = true;
    logger_.Log(LogLevel::DEBUG, "Color blend state set in builder");
    return *this;
}

// ─────────────────────────────────────────────────────────────────────────────
// Depth Stencil
// ─────────────────────────────────────────────────────────────────────────────

PipelineBuilder& PipelineBuilder::SetDepthStencil(VkBool32 depthTest, VkBool32 depthWrite, VkCompareOp compareOp) {
    depthStencil_ = {};
    depthStencil_.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil_.depthTestEnable       = depthTest;
    depthStencil_.depthWriteEnable      = depthWrite;
    depthStencil_.depthCompareOp        = compareOp;
    depthStencil_.depthBoundsTestEnable = VK_FALSE;
    depthStencil_.stencilTestEnable     = VK_FALSE;

    hasDepthStencil_ = true;
    logger_.Log(LogLevel::DEBUG, "Depth stencil state set in builder");
    return *this;
}

// ─────────────────────────────────────────────────────────────────────────────
// Viewport State
// ─────────────────────────────────────────────────────────────────────────────

PipelineBuilder& PipelineBuilder::SetViewportState(uint32_t viewportCount, uint32_t scissorCount) {
    viewportState_ = {};
    viewportState_.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState_.viewportCount = viewportCount;
    viewportState_.scissorCount  = scissorCount;

    hasViewportState_ = true;
    logger_.Log(LogLevel::DEBUG, "Viewport state set in builder");
    return *this;
}

// ─────────────────────────────────────────────────────────────────────────────
// Dynamic States
// ─────────────────────────────────────────────────────────────────────────────

PipelineBuilder& PipelineBuilder::SetDynamicStates(const std::vector<VkDynamicState>& dynamicStates) {
    dynamicStatesStorage_ = dynamicStates;

    dynamicState_ = {};
    dynamicState_.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState_.dynamicStateCount = static_cast<uint32_t>(dynamicStatesStorage_.size());
    dynamicState_.pDynamicStates    = dynamicStatesStorage_.data();

    hasDynamicStates_ = true;
    logger_.Log(LogLevel::DEBUG, "Dynamic states set in builder");
    return *this;
}

// ─────────────────────────────────────────────────────────────────────────────
// Render Pass & Layout
// ─────────────────────────────────────────────────────────────────────────────

PipelineBuilder& PipelineBuilder::SetRenderPass(VkRenderPass renderPass, uint32_t subpass) {
    renderPass_ = renderPass;
    subpass_    = subpass;

    hasRenderPass_ = true;
    logger_.Log(LogLevel::DEBUG, "Render pass set in builder");
    return *this;
}

PipelineBuilder& PipelineBuilder::SetPipelineLayout(uint32_t setLayoutCount, const VkDescriptorSetLayout* pSetLayouts,
                                                    uint32_t pushConstantRangeCount, const VkPushConstantRange* pPushConstantRanges) {
    PipelineUtils::ValidateDescriptorSetLayouts(setLayoutCount, pSetLayouts, logger_);
    PipelineUtils::ValidatePushConstantRanges(pushConstantRangeCount, pPushConstantRanges, logger_);

    logger_.Log(LogLevel::DEBUG, "SetPipelineLayout: {} descriptor set layout(s), {} push constant range(s)",
                setLayoutCount, pushConstantRangeCount);

    VkPipelineLayoutCreateInfo info{};
    info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    info.setLayoutCount         = setLayoutCount;
    info.pSetLayouts            = pSetLayouts;
    info.pushConstantRangeCount = pushConstantRangeCount;
    info.pPushConstantRanges    = pPushConstantRanges;

    if (vkCreatePipelineLayout(device_, &info, nullptr, &pipelineLayout_) != VK_SUCCESS) {
        logger_.Log(LogLevel::EXCEPT, "SetPipelineLayout: vkCreatePipelineLayout failed");
        throw std::runtime_error("PipelineBuilder: failed to create pipeline layout");
    }

    logger_.Log(LogLevel::DEBUG, "Pipeline layout created successfully");
    hasPipelineLayout_ = true;
    return *this;
}

// ─────────────────────────────────────────────────────────────────────────────
// Build
// ─────────────────────────────────────────────────────────────────────────────

std::unique_ptr<Pipeline> PipelineBuilder::Build() {
    // Collect all missing states in one shot so the caller sees everything at once
    std::ostringstream missing;
    if (!hasShaderStages_)   missing << "\n  - AddShaderStage()";
    if (!hasVertexInput_)    missing << "\n  - SetVertexInput()";
    if (!hasInputAssembly_)  missing << "\n  - SetInputAssembly()";
    if (!hasRasterizer_)     missing << "\n  - SetRasterizer()";
    if (!hasMultisampling_)  missing << "\n  - SetMultisampling()";
    if (!hasColorBlend_)     missing << "\n  - SetColorBlend()";
    if (!hasDepthStencil_)   missing << "\n  - SetDepthStencil()";
    if (!hasViewportState_)  missing << "\n  - SetViewportState()";
    if (!hasDynamicStates_)  missing << "\n  - SetDynamicStates()";
    if (!hasRenderPass_)     missing << "\n  - SetRenderPass()";
    if (!hasPipelineLayout_) missing << "\n  - SetPipelineLayout()";

    const std::string missingStr = missing.str();
    if (!missingStr.empty())
        throw std::runtime_error("PipelineBuilder::Build() — missing required state(s):" + missingStr);

    logger_.Log(LogLevel::DEBUG, "All required pipeline states set. Building pipeline...");

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount          = static_cast<uint32_t>(shaderStages_.size());
    pipelineInfo.pStages             = shaderStages_.data();
    pipelineInfo.pVertexInputState   = &vertexInputInfo_;
    pipelineInfo.pInputAssemblyState = &inputAssembly_;
    pipelineInfo.pViewportState      = &viewportState_;
    pipelineInfo.pRasterizationState = &rasterizer_;
    pipelineInfo.pMultisampleState   = &multisampling_;
    pipelineInfo.pDepthStencilState  = &depthStencil_;
    pipelineInfo.pColorBlendState    = &colorBlending_;
    pipelineInfo.pDynamicState       = &dynamicState_;
    pipelineInfo.layout              = pipelineLayout_;
    pipelineInfo.renderPass          = renderPass_;
    pipelineInfo.subpass             = subpass_;

    auto pipeline = std::make_unique<Pipeline>(logger_);
    pipeline->Initialize(device_, pipelineInfo);

    return pipeline;
}