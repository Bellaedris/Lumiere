//
// Created by belle on 25/02/2026.
//

#include "MeshRenderer.h"

#include "Lumiere/Utils/MeshLoader.h"

namespace lum::comp
{
MeshRenderer::MeshRenderer()
{
    m_fileBrowser.SetTitle("Mesh selection");
    m_fileBrowser.SetTypeFilters({".gltf", ".glb", ".obj", ".fbx"});
}

void MeshRenderer::DrawInspector()
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanFullWidth;
    if (ImGui::TreeNodeEx("Mesh Renderer", flags))
    {
        if (m_mesh != nullptr)
            ImGui::Text("Current Mesh: %s", m_mesh->Name().c_str());
        if (ImGui::Button("Pick a mesh"))
        {
            // load a mesh from file/resourcesManager
            m_fileBrowser.Open();
        }
        ImGui::TreePop();
    }

    m_fileBrowser.Display();
    if (m_fileBrowser.HasSelected())
    {
        utils::MeshLoader loader;
        m_mesh = loader.LoadMeshFromFile(m_fileBrowser.GetSelected().string());
        m_fileBrowser.ClearSelected();
    }
}
} // lum::comp