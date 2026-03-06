//
// Created by belle on 25/02/2026.
//

#include "MeshRenderer.h"

#include "Lumiere/ScriptEngine.h"
#include "Lumiere/Utils/MeshLoader.h"

namespace lum::comp
{
bool MeshRenderer::m_registered = false;

void MeshRenderer::RegisterType()
{
    sol::state& lua = ScriptEngine::Instance();

    sol::usertype<gfx::Mesh> mesh = lua.new_usertype<gfx::Mesh>("Mesh");
    mesh["name"] = &gfx::Mesh::Name;

    sol::usertype<MeshRenderer> type = lua.new_usertype<MeshRenderer>("MeshRenderer");
    type["mesh"] = sol::property([](MeshRenderer& m) { return m.m_mesh; });

}

MeshRenderer::MeshRenderer(Node3D *node)
    : IComponent(node)
{
    if (m_registered == false)
        RegisterType();

    m_fileBrowser.SetTitle("Mesh selection");
    m_fileBrowser.SetTypeFilters({".gltf", ".glb", ".obj", ".fbx"});
}

void MeshRenderer::SetMesh(const std::string &path)
{
    utils::MeshLoader loader;
    m_mesh = loader.LoadMeshFromFile(path);
}

void MeshRenderer::DrawInspector()
{
    static ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen;
    if (ImGui::TreeNodeEx(ICON_FA_CUBE " Mesh Renderer", flags))
    {
        if (ImGui::Button("Pick a mesh"))
        {
            // load a mesh from file/resourcesManager
            m_fileBrowser.Open();
        }
        if (Mesh() != nullptr)
        {
            InspectorDrawMeshDetails();
        }
        ImGui::TreePop();
    }

    m_fileBrowser.Display();
    if (m_fileBrowser.HasSelected())
    {
        SetMesh(m_fileBrowser.GetSelected().string());
        m_fileBrowser.ClearSelected();
    }
}

void MeshRenderer::InspectorDrawMeshDetails()
{
    ImGui::Text("Current Mesh: %s", Mesh()->Name().c_str());
    if (ImGui::TreeNodeEx("Submeshes", ImGuiTreeNodeFlags_DrawLinesFull))
    {
        std::vector<lum::gfx::SubMesh>& submeshes = Mesh()->Primitives();
        for (auto& submesh : submeshes)
        {
            if (ImGui::TreeNodeEx(submesh.Name().c_str(), ImGuiTreeNodeFlags_DrawLinesFull))
            {
                submesh.Material()->DrawEditor();
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }
}

} // lum::comp