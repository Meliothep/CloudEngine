#pragma once
#include "pch.h"

class ShaderModule {
public:
    ShaderModule(Logger& logger) : logger_(logger) {}
    ~ShaderModule() = default;

    void Initialize(VkDevice device, const std::string& path);
    void Shutdown();

    VkShaderModule GetModule() const { return module_; }
    VkShaderStageFlagBits GetStage() const { return stage_; }

private:
    std::vector<char> GetSource(const std::string path);

    Logger& logger_;

    VkDevice device_{VK_NULL_HANDLE};
    VkShaderModule module_{VK_NULL_HANDLE};
    VkShaderStageFlagBits stage_;
};