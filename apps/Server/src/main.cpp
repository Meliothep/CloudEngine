#include <VulkanRenderer/VulkanRenderer.hpp>
#include <enet/enet.h>

int main() {
    VulkanRenderer renderer;
    renderer.hello();

    if (enet_initialize () != 0)
    {
        fprintf (stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }
    atexit (enet_deinitialize);
    return 0;
}
