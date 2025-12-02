#include "VulkanRenderer/Pipeline/RenderPipelineManager.hpp"
#include "VulkanRenderer/Ressource/Vertex.hpp"

#include <stdexcept>
#include <filesystem>
#include <array>
#include "VulkanRenderer/Utils/FileUtils.hpp"

void RenderPipelineManager::CreateShaderModule(const std::vector<char>& code, VkShaderModule& outModule) {
    if (code.empty()) {
        throw std::runtime_error("Shader code is empty");
    }
    if (code.size() % 4 != 0) {
        throw std::runtime_error("Shader code size is not a multiple of 4 - invalid SPIR-V");
    }

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule(device_, &createInfo, nullptr, &outModule) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module");
    }
}

void RenderPipelineManager::LoadShaderModules(const std::string& vertShaderPath,
                                              const std::string& fragShaderPath,
                                              VkShaderModule& outVertModule,
                                              VkShaderModule& outFragModule,
                                              std::vector<char>& outVertCode,
                                              std::vector<char>& outFragCode) {
    try {
        namespace fs = std::filesystem;
        fs::path exeDir = GetExecutableDir();
        fs::path shaderDir = exeDir / "shaders";
        fs::path vertPath = shaderDir / (vertShaderPath + ".spv");
        fs::path fragPath = shaderDir / (fragShaderPath + ".spv");

        outVertCode = ReadFile(vertPath.string());
        outFragCode = ReadFile(fragPath.string());
    } catch(const std::exception& e) {
        logger_.Log(LogLevel::EXCEPT, std::string("Failed to load shader codes: ") + e.what());
        throw;
    }

    logger_.Log(LogLevel::DEBUG, "Shaders codes loaded successfully");

    CreateShaderModule(outVertCode, outVertModule);
    CreateShaderModule(outFragCode, outFragModule);
}

void RenderPipelineManager::CreateGraphicsPipelineLayout() {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0; // Optional
    pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    if (vkCreatePipelineLayout(device_, &pipelineLayoutInfo, nullptr, &pipelineLayout_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void RenderPipelineManager::CreateGraphicsPipelineFromModules(VkShaderModule vertShaderModule,
                                                         VkShaderModule fragShaderModule,
                                                         VkRenderPass renderPass,
                                                         VkExtent2D extent) {
    // --- Shader stages -----------------------------------------------------
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName  = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName  = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    // --- Dynamic states ---------------------------------------------------
    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    // --- Vertex input -----------------------------------------------------
    // Create binding and attribute descriptions that match:
    // struct Vertex { float position[3]; float normal[3]; float uv[2]; };
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding   = 0;
    bindingDescription.stride    = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
    // location 0 -> position (vec3)
    attributeDescriptions[0].binding  = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset   = offsetof(Vertex, position);

    // location 1 -> normal (vec3)
    attributeDescriptions[1].binding  = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset   = offsetof(Vertex, normal);

    // location 2 -> uv (vec2)
    attributeDescriptions[2].binding  = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format   = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset   = offsetof(Vertex, uv);

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount   = 1;
    vertexInputInfo.pVertexBindingDescriptions      = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions    = attributeDescriptions.data();

    // --- Input assembly ---------------------------------------------------
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // --- Viewport state (viewport & scissor are dynamic) ------------------
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount  = 1;

    // --- Rasterizer -------------------------------------------------------
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    // --- Multisampling ----------------------------------------------------
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // --- Color blending ---------------------------------------------------
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    // --- Pipeline create --------------------------------------------------
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout_;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    logger_.Log(LogLevel::INFO, std::format("Creating graphics pipeline with render pass {:#010x}, extent {}x{}",
        reinterpret_cast<intptr_t>(renderPass),
        extent.width,
        extent.height));
    logger_.Log(LogLevel::INFO, std::format("Creating graphics pipeline with Device {:#010x}",
        reinterpret_cast<intptr_t>(device_)));

    if (vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    logger_.Log(LogLevel::INFO, "Graphics pipeline created successfully");
}

void RenderPipelineManager::Initialize(VkDevice device,
                                       VkRenderPass renderPass,
                                       const std::string& vertShaderPath,
                                       const std::string& fragShaderPath,
                                       VkExtent2D extent) {
    device_ = device;
    logger_.Log(LogLevel::INFO, std::format("Creating graphics pipeline with render pass {:#010x}, extent {}x{}",
        reinterpret_cast<intptr_t>(renderPass),
        extent.width,
        extent.height));
    logger_.Log(LogLevel::INFO, "Creating graphics pipeline");

    VkShaderModule vertModule = VK_NULL_HANDLE;
    VkShaderModule fragModule = VK_NULL_HANDLE;
    std::vector<char> vertCode;
    std::vector<char> fragCode;

    // Load shader binaries and create shader modules
    LoadShaderModules(vertShaderPath, fragShaderPath, vertModule, fragModule, vertCode, fragCode);
    
    CreateGraphicsPipelineLayout();

    // Create the graphics pipeline from the loaded modules
    CreateGraphicsPipelineFromModules(vertModule, fragModule, renderPass, extent);
    
    vkDestroyShaderModule(device, vertModule, nullptr);
    vkDestroyShaderModule(device, fragModule, nullptr);
}

void RenderPipelineManager::Shutdown() {
    if (graphicsPipeline_ != VK_NULL_HANDLE) {
        vkDestroyPipeline(device_, graphicsPipeline_, nullptr);
        graphicsPipeline_ = VK_NULL_HANDLE;
    }

    if (pipelineLayout_ != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(device_, pipelineLayout_, nullptr);
        pipelineLayout_ = VK_NULL_HANDLE;
    }

    logger_.Log(LogLevel::INFO, "Render pipeline destroyed successfully");
}
