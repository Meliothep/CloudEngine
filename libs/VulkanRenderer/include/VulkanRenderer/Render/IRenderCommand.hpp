#pragma once

#include <vulkan/vulkan.h>


class IRenderCommand {
public:
    virtual ~IRenderCommand() = default;
    virtual void Record(VkCommandBuffer cmd) = 0;
};
