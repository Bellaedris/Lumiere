//
// Created by Bellaedris on 15/01/2026.
//

#include "MeshLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "../Graphics/Mesh.h"
#include "Lumiere/Graphics/MaterialPBR.h"

namespace lum::utils
{
std::vector<gfx::MeshPtr> MeshLoader::LoadMeshFromFile(const std::string filename)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filename.c_str(),
        aiProcess_JoinIdenticalVertices   |
        aiProcess_SortByPType             |
        aiProcess_FlipUVs                 |
        aiProcess_GenNormals              | // create the normals if not already in the file
        aiProcess_CalcTangentSpace          // create tan/bitan
        );

    if (scene == nullptr)
    {
        std::cerr << "Couldn't load the mesh at " << filename << "\nWith error:" << importer.GetErrorString() << "\n";
        return {};
    }

    directory = filename.substr(0, filename.find_last_of('/'));
    std::vector<gfx::MeshPtr> meshes;
    ProcessNode(scene->mRootNode, scene, meshes);

    return meshes;
}

void MeshLoader::ProcessNode(aiNode *node, const aiScene *scene, std::vector<gfx::MeshPtr> &meshes)
{
    for (uint32_t i = 0; i < node->mNumMeshes; i++)
    {
         meshes.push_back(ProcessMesh(scene->mMeshes[node->mMeshes[i]], scene));
    }
    for (uint32_t i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, meshes);
    }
}

gfx::MeshPtr MeshLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<gfx::Mesh::VertexData> vertices;
    std::vector<uint32_t> indices;

    for (int i = 0; i < mesh->mNumVertices; i++)
    {
        gfx::Mesh::VertexData vertex;
        //positions
        vertex.pos = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};

        // normals
        vertex.normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};

        //texcoords
        // I assume we always will have texcoords here, but it might not always be the case
        vertex.texcoord = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};

        // add Tan/Bitan

        vertices.push_back(vertex);
    }

    // indices
    for (int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // Material
    aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
    // we assume that for PBR materials, we only use the first albedo map.
    aiString str;
    mat->GetTexture(aiTextureType_DIFFUSE, 0, &str);
    gpu::TexturePtr albedo = std::make_shared<gpu::Texture>(gpu::Texture::TextureTarget::Target2D, (directory + "/" + std::string(str.C_Str())).c_str(), true);

    gpu::ShaderPtr pbrShader = std::make_shared<gpu::Shader>();
    pbrShader->AddShaderFromFile(gpu::Shader::Vertex, "shaders/default.vert");
    pbrShader->AddShaderFromFile(gpu::Shader::Fragment, "shaders/default.frag");
    gfx::MaterialPtr pbrMat = std::make_shared<gfx::MaterialPBR>(pbrShader, albedo, nullptr, nullptr, nullptr);

    return std::make_unique<gfx::Mesh>(vertices, indices, pbrMat);
}
} // lum