//
// Created by Bellaedris on 20/01/2026.
//

#include "SceneDesc.h"

#include "Lumiere/Utils/MeshLoader.h"

namespace lum::rdr
{
void SceneDesc::AddMesh(const std::string &path)
{
    utils::MeshLoader loader;
    m_meshes.emplace_back(loader.LoadMeshFromFile(path));
}

void SceneDesc::ForEach(const std::function<void(const gfx::SubMesh &primitive, const gfx::MaterialPtr &material)> &callback) const
{
    for (const auto& mesh : m_meshes)
        for (const gfx::SubMesh& submesh : mesh->Primitives())
            callback(submesh, submesh.Material());
}
} // lum::rdr