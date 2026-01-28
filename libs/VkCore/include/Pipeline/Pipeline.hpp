#pragma once
#include "pch.h"

struct PipelineShaderStage {
    VkShaderModule module;
    VkShaderStageFlagBits stage;
};

class Pipeline {
public:
    Pipeline(Logger& logger) : logger_(logger) {}
    ~Pipeline() = default;

    void Initialize(VkDevice device,
                    VkRenderPass renderPass,
                    const PipelineShaderStage& vertStage,
                    const PipelineShaderStage& fragStage,
                    VkExtent2D extent);

    void Shutdown();

    VkPipeline GetPipeline() const { return graphicsPipeline_; }
    VkPipelineLayout GetPipelineLayout() const { return pipelineLayout_; }

private:
    Logger& logger_;

    VkDevice device_{VK_NULL_HANDLE};
    VkPipelineLayout pipelineLayout_{VK_NULL_HANDLE};
    VkPipeline graphicsPipeline_{VK_NULL_HANDLE};
    
    void CreateGraphicsPipelineLayout();
    
    void CreateGraphicsPipelineFromModules(const PipelineShaderStage& vertStage,
                                           const PipelineShaderStage& fragStage,
                                           VkRenderPass renderPass,
                                           VkExtent2D extent);
};
