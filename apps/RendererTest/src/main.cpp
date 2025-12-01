#include <Utils/Logger.hpp>
#include <Utils/Win32ConsoleStrategy.hpp>
#include <Windows/Win32Window.hpp>

#include <VulkanRenderer/VulkanRenderer.hpp>

int main() {
    auto logger = std::make_unique<Logger>();

    logger->AddLoggerStrat(std::make_unique<Win32ConsoleStrategy>(LogLevel::DEBUG));

    IWindow* window = new Win32Window(500, 500, "TestWindow"); 
    //IWindow* window = nullptr;    

    VulkanRenderer renderer(*logger);
    renderer.Initialize(window);
    while(window->ProcessMessages()){
        renderer.DrawFrame();
    }

    renderer.Shutdown();

    return 0;
}
