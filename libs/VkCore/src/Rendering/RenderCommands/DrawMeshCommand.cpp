#include "Rendering/RenderCommands/DrawMeshCommand.hpp"
#include "Rendering/RenderQueue.hpp"

#include "Resources/Mesh.hpp"
#include "Resources/Material.hpp"


DrawMeshCommand::DrawMeshCommand(Mesh* mesh, Material* material, const void* transformPtr, size_t transformSize)
    : mesh_(mesh), material_(material), transformPtr_(transformPtr), transformSize_(transformSize) {}

void DrawMeshCommand::Record(VkCommandBuffer cmd) {
    if (!mesh_ || !material_) return;

    // Bind material first (pipeline + descriptors)
    material_->Bind(cmd);

    if (transformPtr_ && transformSize_ > 0) {
        VkPipelineLayout layout = material_->GetPipelineLayout();
        vkCmdPushConstants(cmd, layout, VK_SHADER_STAGE_VERTEX_BIT, 0, static_cast<uint32_t>(transformSize_), transformPtr_);
    }

    mesh_->Bind(cmd);

    if (mesh_->HasIndices()) {
        vkCmdDrawIndexed(cmd, mesh_->GetIndexCount(), 1, 0, 0, 0);
    } else {
        vkCmdDraw(cmd, mesh_->GetVertexCount(), 1, 0, 0);
    }
}
