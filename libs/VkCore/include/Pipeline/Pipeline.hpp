#pragma once
#include "pch.h"

class Pipeline {
public:
    Pipeline(Logger& logger) : logger_(logger) {}
    ~Pipeline() = default;

    void Initialize(VkDevice device,
                    VkRenderPass renderPass,
                    VkShaderModule vertShaderModule,
                    VkShaderModule fragShaderModule,
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
    
    void CreateGraphicsPipelineFromModules(VkShaderModule vertShaderModule,
                                           VkShaderModule fragShaderModule,
                                           VkRenderPass renderPass,
                                           VkExtent2D extent);
};
