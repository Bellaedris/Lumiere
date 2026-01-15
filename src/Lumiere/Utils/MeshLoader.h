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

    void         ProcessNode(aiNode* node, const aiScene* scene, std::vector<gfx::MeshPtr> &meshes);
    gfx::MeshPtr ProcessMesh(aiMesh* mesh, const aiScene* scene);
public:
    MeshLoader() = default;
    std::vector<gfx::MeshPtr>  LoadMeshFromFile(const std::string filename);
};
} // lum