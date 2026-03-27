#include "Pipeline/Pipeline.hpp"
#include <stdexcept>

void Pipeline::CreateLayout(VkDevice device, uint32_t setLayoutCount, const VkDescriptorSetLayout* pSetLayouts, 
                            uint32_t pushConstantRangeCount, const VkPushConstantRange* pPushConstantRanges) {
    device_ = device;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = setLayoutCount;
    pipelineLayoutInfo.pSetLayouts = pSetLayouts;
    pipelineLayoutInfo.pushConstantRangeCount = pushConstantRangeCount;
    pipelineLayoutInfo.pPushConstantRanges = pPushConstantRanges;

    if (vkCreatePipelineLayout(device_, &pipelineLayoutInfo, nullptr, &pipelineLayout_) != VK_SUCCESS) {
        logger_.Log(LogLevel::EXCEPT, "Failed to create pipeline layout");
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void Pipeline::Initialize(VkDevice device, const VkGraphicsPipelineCreateInfo& pipelineInfo) {
    device_ = device;
    
    // Safety check: if layout wasn't created externally, use the one passed in info
    if (pipelineLayout_ == VK_NULL_HANDLE) {
        pipelineLayout_ = pipelineInfo.layout;
    }

    if (vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline_) != VK_SUCCESS) {
        logger_.Log(LogLevel::EXCEPT, "Failed to create graphics pipeline");
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    logger_.Log(LogLevel::INFO, "Graphics pipeline created successfully via Builder");
}

void Pipeline::Shutdown() {
    if (graphicsPipeline_ != VK_NULL_HANDLE) {
        vkDestroyPipeline(device_, graphicsPipeline_, nullptr);
        graphicsPipeline_ = VK_NULL_HANDLE;
    }

    if (pipelineLayout_ != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(device_, pipelineLayout_, nullptr);
        pipelineLayout_ = VK_NULL_HANDLE;
    }

    logger_.Log(LogLevel::INFO, "Pipeline resources cleared");
}