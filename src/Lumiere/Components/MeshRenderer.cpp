//
// Created by belle on 25/02/2026.
//

#include "MeshRenderer.h"

#include "Lumiere/ScriptEngine.h"
#include "Lumiere/Utils/MeshLoader.h"

namespace lum::comp
{
REGISTER_TO_COMPONENT_FACTORY(MeshRenderer, "MeshRenderer");
bool MeshRenderer::m_typeRegistered = false;

void MeshRenderer::RegisterType()
{
    sol::state& lua = ScriptEngine::Instance();

    sol::usertype<gfx::Mesh> mesh = lua.new_usertype<gfx::Mesh>("Mesh");
    mesh["name"] = &gfx::Mesh::Name;

    sol::usertype<MeshRenderer> type = lua.new_usertype<MeshRenderer>("MeshRenderer");
    type["mesh"] = sol::property([](MeshRenderer& m) { return m.m_mesh; });

    m_typeRegistered = true;
}

MeshRenderer::MeshRenderer(Node3D *node)
    : IComponent(node)
{
    if (m_typeRegistered == false)
        RegisterType();
}

void MeshRenderer::SetMesh(const std::string &path)
{
    utils::MeshLoader loader;
    m_mesh = loader.LoadMeshFromFile(path);
}

void MeshRenderer::Serialize(YAML::Node node)
{
    YAML::Node mr;
    mr["componentType"] = "MeshRenderer";
    mr["path"] = m_mesh == nullptr ? "" : m_mesh->Path();
    node.push_back(mr);
}

void MeshRenderer::Deserialize(YAML::Node node)
{
    SetMesh(node["path"].as<std::string>());
}
} // lum::comp