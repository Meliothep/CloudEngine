#pragma once
#include "Utils/Logger.hpp"
#include "Windows/IWindow.hpp"

class VulkanRenderer{
public:
    VulkanRenderer(Logger& logger): logger_(logger){}
    
    void Initialize(IWindow* window = nullptr);
private:
    Logger& logger_;
};