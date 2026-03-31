#include "Pipeline/Pipeline.hpp"
#include <stdexcept>

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