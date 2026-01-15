//
// Created by Bellaedris on 15/01/2026.
//

#include "Actor.h"

#include "Utils/FileUtils.h"

namespace lum
{
Actor::Actor(const std::string &path)
    : meshes(std::move(utils::FileUtils::LoadMeshFromFile(path)))
{}

void Actor::Draw() const
{
    for (const MeshPtr& mesh : meshes)
        mesh->Draw();
}
} // lum