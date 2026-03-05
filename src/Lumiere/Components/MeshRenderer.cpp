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
}

void MeshRenderer::SetMesh(const std::string &path)
{
    utils::MeshLoader loader;
    m_mesh = loader.LoadMeshFromFile(path);
}

} // lum::comp