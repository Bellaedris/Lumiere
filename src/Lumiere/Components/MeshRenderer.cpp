//
// Created by belle on 25/02/2026.
//

#include "MeshRenderer.h"

#include "../Systems/ScriptEngine.h"
#include "Lumiere/ResourcesManager.h"
#include "Lumiere/Utils/MeshLoader.h"

namespace lum::comp
{
REGISTER_TO_COMPONENT_FACTORY(MeshRenderer, "MeshRenderer");
bool MeshRenderer::m_typeRegistered = false;

void MeshRenderer::RegisterType(SystemProvider* systems)
{
    sol::state& lua =  systems->m_scripting->State();

    sol::usertype<gfx::Mesh> mesh = lua.new_usertype<gfx::Mesh>("Mesh");
    mesh["name"] = &gfx::Mesh::Name;

    sol::usertype<MeshRenderer> type = lua.new_usertype<MeshRenderer>("MeshRenderer");
    type["mesh"] = sol::property([](MeshRenderer& m) { return m.m_mesh; });

    m_typeRegistered = true;
}

MeshRenderer::MeshRenderer(Node3D *node, SystemProvider* systems)
    : IComponent(node, systems)
{
    if (m_typeRegistered == false)
        RegisterType(systems);
}

void MeshRenderer::SetMesh(const std::string &path)
{
    if (m_mesh != nullptr && (path == m_mesh->Path() || path.empty() == false))
        return;

    utils::MeshLoader loader;
    m_mesh = loader.LoadMeshFromFile(path);
    m_materials.clear();
    for (const auto& primitive : m_mesh->Primitives())
    {
        std::optional<gfx::MaterialPtr> mat = ResourcesManager::Instance()->GetMaterial(primitive.DefaultMat());
        if (mat.has_value() == false)
            m_materials.push_back(ResourcesManager::Instance()->GetMaterial(ResourcesManager::DEFAULT_MATERIAL_PBR_LIT).value());
        else
            m_materials.push_back(mat.value());
    }
}

void MeshRenderer::Serialize(YAML::Node& node)
{
    YAML::Node mr;
    mr["componentType"] = "MeshRenderer";
    mr["path"] = m_mesh == nullptr ? "" : m_mesh->Path();
    YAML::Node materials;
    for (const auto& mat : m_materials)
        materials.push_back(YAML::Node(mat->Name()));
    mr["materials"] = materials;
    node.push_back(mr);
}

void MeshRenderer::Deserialize(YAML::Node& node)
{
    std::string path = node["path"].as<std::string>();
    if (path.empty() == false)
    {
        utils::MeshLoader loader;
        m_mesh = loader.LoadMeshFromFile(path);
    }
    for (const auto& materialNode : node["materials"])
    {
        std::optional<gfx::MaterialPtr> mat = ResourcesManager::Instance()->GetMaterial(materialNode.as<std::string>());
        if (mat.has_value() == false)
            m_materials.push_back(ResourcesManager::Instance()->GetMaterial(ResourcesManager::DEFAULT_MATERIAL_PBR_LIT).value());
        else
            m_materials.push_back(mat.value());
    }
}
} // lum::comp