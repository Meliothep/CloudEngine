#pragma once
#include "pch.h"

class RenderPipelineManager {
public:
    RenderPipelineManager(Logger& logger) : logger_(logger) {}
    ~RenderPipelineManager() = default;

    void Initialize(VkDevice device,
                    VkRenderPass renderPass,
                    const std::string& vertShaderPath,
                    const std::string& fragShaderPath,
                    VkExtent2D extent);

    void Shutdown();

    VkPipeline GetPipeline() const { return graphicsPipeline_; }
    VkPipelineLayout GetPipelineLayout() const { return pipelineLayout_; }

private:
    Logger& logger_;

    VkDevice device_{VK_NULL_HANDLE};
    VkPipelineLayout pipelineLayout_{VK_NULL_HANDLE};
    VkPipeline graphicsPipeline_{VK_NULL_HANDLE};

    void CreateShaderModule(const std::vector<char>& code, VkShaderModule& outModule);

    void LoadShaderModules(const std::string& vertShaderPath,
                           const std::string& fragShaderPath,
                           VkShaderModule& outVertModule,
                           VkShaderModule& outFragModule,
                           std::vector<char>& outVertCode,
                           std::vector<char>& outFragCode);
    
    void CreateGraphicsPipelineLayout();
    
    void CreateGraphicsPipelineFromModules(VkShaderModule vertShaderModule,
                                           VkShaderModule fragShaderModule,
                                           VkRenderPass renderPass,
                                           VkExtent2D extent);
};
