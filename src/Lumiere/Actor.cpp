//
// Created by Bellaedris on 15/01/2026.
//

#include "Actor.h"

#include "Utils/FileUtils.h"
#include "Utils/MeshLoader.h"

namespace lum
{
Actor::Actor(const std::string &path)
{
    utils::MeshLoader loader;
    m_meshes = std::move(loader.LoadMeshFromFile(path));
}

void Actor::Draw() const
{
    for (const gfx::MeshPtr& mesh : m_meshes)
        mesh->Draw();
}
} // lum