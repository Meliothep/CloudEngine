#include "Pipeline/ShaderModule.hpp"

#include <stdexcept>
#include <filesystem>
#include <array>

#include "Utils/FileUtils.hpp"

std::vector<char> ShaderModule::GetSource(const std::string path){
    try {
        namespace fs = std::filesystem;
        fs::path exeDir = GetExecutableDir();
        fs::path shaderDir = exeDir / "shaders";
        fs::path shaderpath = shaderDir / (path + ".spv");

        const std::vector<char> code = ReadFile(shaderpath.string());
        return code; 
    } catch(const std::exception& e) {
        logger_.Log(LogLevel::EXCEPT, std::string("Failed to load shader codes: ") + e.what());
        throw;
    }
}

void ShaderModule::Initialize(VkDevice device, const std::string& path){
    this->device_ = device;

    const std::vector<char> code = GetSource(path);
    
    if (code.empty()) {
        throw std::runtime_error("Shader code is empty");
    }
    if (code.size() % 4 != 0) {
        throw std::runtime_error("Shader code size is no a multiple of 4 - invalid SPIR-V");
    }

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule(device_, &createInfo, nullptr, &module_) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module");
    }
}

void ShaderModule::Shutdown(){
    vkDestroyShaderModule(device_, module_, nullptr);
}