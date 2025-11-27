#pragma once

#include <optional>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    std::optional<uint32_t> computeFamily;
    std::optional<uint32_t> transferFamily;

    bool IsComplete(bool enableScreen) const {
        if (enableScreen) {
            return graphicsFamily.has_value() && presentFamily.has_value();
        } else {
            return graphicsFamily.has_value();
        }
    }
};