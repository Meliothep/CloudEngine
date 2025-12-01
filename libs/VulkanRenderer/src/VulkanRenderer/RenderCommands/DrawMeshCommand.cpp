#include "VulkanRenderer/RenderCommands/DrawMeshCommand.hpp"
#include "VulkanRenderer/Render/RenderQueue.hpp"

#include "VulkanRenderer/Ressource/Mesh.hpp"
#include "VulkanRenderer/Render/Material.hpp"


DrawMeshCommand::DrawMeshCommand(Mesh* mesh, Material* material, const void* transformPtr, size_t transformSize)
    : mesh_(mesh), material_(material), transformPtr_(transformPtr), transformSize_(transformSize) {}

void DrawMeshCommand::Record(VkCommandBuffer cmd) {
    if (!mesh_ || !material_) return;

    // Bind material first (pipeline + descriptors)
    material_->Bind(cmd);

    // Optionally push constants if transform provided and material defines push constant ranges.
    // This is a small, flexible pattern: if material expects a push-constant for the model matrix,
    // it should declare it in the pipeline layout; here we simply push it.
    if (transformPtr_ && transformSize_ > 0) {
        VkPipelineLayout layout = material_->GetPipelineLayout();
        // TODO: ensure the pipeline layout has a push-constant range with VK_SHADER_STAGE_VERTEX_BIT and correct size
        vkCmdPushConstants(cmd, layout, VK_SHADER_STAGE_VERTEX_BIT, 0, static_cast<uint32_t>(transformSize_), transformPtr_);
    }

    // Bind mesh buffers (vertex/index). Mesh::Bind should call vkCmdBindVertexBuffers / vkCmdBindIndexBuffer
    mesh_->Bind(cmd);

    // Issue draw call
    if (mesh_->HasIndices()) {
        vkCmdDrawIndexed(cmd, mesh_->GetIndexCount(), 1, 0, 0, 0);
    } else {
        // if your mesh has vertex count but no indices:
        vkCmdDraw(cmd, mesh_->GetVertexCount(), 1, 0, 0);
    }
}
