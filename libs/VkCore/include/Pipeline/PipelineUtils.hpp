#pragma once
#include "pch.h"

namespace PipelineUtils {

inline void ValidateDescriptorSetLayouts(uint32_t count, const VkDescriptorSetLayout* layouts, Logger& logger) {
    if (count > 0 && layouts == nullptr) {
        logger.Log(LogLevel::EXCEPT, "SetPipelineLayout: setLayoutCount={} but pSetLayouts is nullptr", count);
        throw std::invalid_argument("PipelineBuilder: setLayoutCount > 0 but pSetLayouts is nullptr");
    }
    if (count == 0 && layouts != nullptr)
        logger.Log(LogLevel::WARN, "SetPipelineLayout: pSetLayouts is set but setLayoutCount=0, will be ignored");
}

inline void ValidatePushConstantRanges(uint32_t count, const VkPushConstantRange* ranges, Logger& logger) {
    if (count > 0 && ranges == nullptr) {
        logger.Log(LogLevel::EXCEPT, "SetPipelineLayout: pushConstantRangeCount={} but pPushConstantRanges is nullptr", count);
        throw std::invalid_argument("PipelineBuilder: pushConstantRangeCount > 0 but pPushConstantRanges is nullptr");
    }
    if (count == 0 && ranges != nullptr) {
        logger.Log(LogLevel::WARN, "SetPipelineLayout: pPushConstantRanges is set but pushConstantRangeCount=0, will be ignored");
        return;
    }
    for (uint32_t i = 0; i < count; ++i) {
        const VkPushConstantRange& range = ranges[i];
        if (range.size == 0) {
            logger.Log(LogLevel::EXCEPT, "SetPipelineLayout: push constant range[{}] has size=0", i);
            throw std::invalid_argument("PipelineBuilder: push constant range size cannot be 0");
        }
        if (range.size % 4 != 0 || range.offset % 4 != 0) {
            logger.Log(LogLevel::EXCEPT, "SetPipelineLayout: push constant range[{}] offset={} size={} — must be multiples of 4", i, range.offset, range.size);
            throw std::invalid_argument("PipelineBuilder: push constant range offset and size must be multiples of 4");
        }
        if (range.stageFlags == 0) {
            logger.Log(LogLevel::EXCEPT, "SetPipelineLayout: push constant range[{}] has no stage flags set", i);
            throw std::invalid_argument("PipelineBuilder: push constant range stageFlags cannot be 0");
        }
    }
}

} // namespace PipelineUtils