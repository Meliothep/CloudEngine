#pragma once
#include "pch.h"

#include "../Render/IRenderCommand.hpp"

class Mesh;
class Material;

class DrawMeshCommand : public IRenderCommand {
public:
    DrawMeshCommand(Mesh* mesh, Material* material, const void* transformPtr = nullptr, size_t transformSize = 0);
    ~DrawMeshCommand() override = default;

    void Record(VkCommandBuffer cmd) override;

private:
    Mesh* mesh_;
    Material* material_;
    const void* transformPtr_;
    size_t transformSize_;
};
