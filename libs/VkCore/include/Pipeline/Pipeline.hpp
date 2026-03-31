#pragma once
#include "pch.h"

struct PipelineShaderStage {
    VkShaderModule module;
    VkShaderStageFlagBits stage;
};

class Pipeline {
public:
    Pipeline(Logger& logger) : logger_(logger) {}
    ~Pipeline() = default; // Ensure cleanup

    // NEW: The primary entry point for the Builder
    void Initialize(VkDevice device, const VkGraphicsPipelineCreateInfo& pipelineInfo);

    void Shutdown();

    VkPipeline GetPipeline() const { return graphicsPipeline_; }
    VkPipelineLayout GetPipelineLayout() const { return pipelineLayout_; }

private:
    Logger& logger_;
    VkDevice device_{VK_NULL_HANDLE};
    VkPipelineLayout pipelineLayout_{VK_NULL_HANDLE};
    VkPipeline graphicsPipeline_{VK_NULL_HANDLE};
};