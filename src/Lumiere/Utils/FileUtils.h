//
// Created by Bellaedris on 12/12/2025.
//

#pragma once

#include <string>
#include <optional>

#include "assimp/scene.h"
#include "Lumiere/Actor.h"

namespace lum::utils
{
    class FileUtils
    {
    private:
        #pragma region Mesh Loading
        static void ProcessNode(aiNode* node, const aiScene* scene, std::vector<MeshPtr> &meshes);
        static MeshPtr ProcessMesh(aiMesh* mesh, const aiScene* scene);
        #pragma endregion Mesh Loading

    public:
        static std::optional<std::string> read_file(const char* filename);
        static std::vector<MeshPtr> LoadMeshFromFile(const std::string filename);
    };

} // mgl::utils