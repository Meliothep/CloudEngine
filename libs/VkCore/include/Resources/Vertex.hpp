#include "pch.h"

struct Vertex {
    float position[3];
    float normal[3];
    float uv[2];

    // ---- Binding description (1 per vertex type) ----
    static VkVertexInputBindingDescription GetBindingDescription() {
        VkVertexInputBindingDescription binding{};
        binding.binding = 0;
        binding.stride = sizeof(Vertex);
        binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return binding;
    }

    static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributes{};

        // position (vec3) -> location 0
        attributes[0].binding  = 0;
        attributes[0].location = 0;
        attributes[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
        attributes[0].offset   = offsetof(Vertex, position);

        // normal (vec3) -> location 1
        attributes[1].binding  = 0;
        attributes[1].location = 1;
        attributes[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
        attributes[1].offset   = offsetof(Vertex, normal);

        // uv (vec2) -> location 2
        attributes[2].binding  = 0;
        attributes[2].location = 2;
        attributes[2].format   = VK_FORMAT_R32G32_SFLOAT;
        attributes[2].offset   = offsetof(Vertex, uv);

        return attributes;
    }
};
