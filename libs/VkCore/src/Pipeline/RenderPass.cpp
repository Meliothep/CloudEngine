#include "Pipeline/RenderPass.hpp"

void RenderPass::Initialize(VkDevice device, VkFormat swapchainImageFormat) {
    device_ = device;

    // Define the color attachment (the swapchain image)
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapchainImageFormat;                  
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;                
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; 
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // Create a reference to that attachment from a subpass
    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // Define a single subpass that uses this color attachment
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    // Define a dependency to handle layout transitions automatically
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    // Assemble the render pass info
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VkResult res = vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass_);
    if (res != VK_SUCCESS) {
        logger_.Log(LogLevel::EXCEPT, std::string("Failed to create render pass!, VkResult=") + std::to_string(static_cast<int>(res)));
        throw std::runtime_error("Failed to create render pass!");
    }

    logger_.Log(LogLevel::INFO, "Render pass created successfully");
}

void RenderPass::Shutdown() {
    if (renderPass_ != VK_NULL_HANDLE) {
        vkDestroyRenderPass(device_, renderPass_, nullptr);
        renderPass_ = VK_NULL_HANDLE;
    }
    logger_.Log(LogLevel::INFO, "Render pass destroyed successfully");
}

void RenderPass::BeginRenderPass(VkCommandBuffer cmdBuffer, VkFramebuffer framebuffer, VkExtent2D extent, const VkClearValue* clearValues, uint32_t clearValueCount) {
    if (renderPass_ == VK_NULL_HANDLE) {
        logger_.Log(LogLevel::EXCEPT, "BeginRenderPass called but renderPass_ is VK_NULL_HANDLE");
        throw std::runtime_error("Render pass not created");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass_;
    renderPassInfo.framebuffer = framebuffer;

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = extent;

    renderPassInfo.clearValueCount = clearValueCount;
    renderPassInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderPass::EndRenderPass(VkCommandBuffer cmdBuffer) {
    vkCmdEndRenderPass(cmdBuffer);
}
