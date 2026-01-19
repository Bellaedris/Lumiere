//
// Created by Bellaedris on 15/01/2026.
//

#pragma once
#include <string>

#include "Lumiere/Graphics/Mesh.h"
#include "assimp/scene.h"

namespace lum::utils
{
class MeshLoader
{
private:
    std::string directory;

    void         ProcessNode(aiNode* node, const aiScene* scene, std::vector<gfx::SubMesh> &subMeshes);

    gfx::SubMesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
public:
    MeshLoader() = default;

    gfx::MeshPtr LoadMeshFromFile(const std::string &filename);
};
} // lum