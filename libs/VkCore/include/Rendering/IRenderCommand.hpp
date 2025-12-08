#pragma once
#include "pch.h"

class IRenderCommand {
public:
    virtual ~IRenderCommand() = default;
    virtual void Record(VkCommandBuffer cmd) = 0;
};
