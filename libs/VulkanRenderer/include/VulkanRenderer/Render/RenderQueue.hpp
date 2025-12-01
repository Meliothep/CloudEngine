#pragma once

#include "VulkanRenderer/Render/IRenderCommand.hpp"

#include <vector>
#include <memory>

class RenderQueue {
public:
    RenderQueue() = default;
    ~RenderQueue() = default;

    void Submit(IRenderCommand* cmd) {
        commands_.push_back(cmd);
    }

    const std::vector<IRenderCommand*>& GetCommands() const { return commands_; }
    void Clear() { commands_.clear(); }

private:
    std::vector<IRenderCommand*> commands_;
};
