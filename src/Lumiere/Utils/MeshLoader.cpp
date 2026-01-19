//
// Created by Bellaedris on 15/01/2026.
//

#include "MeshLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "../Graphics/Mesh.h"
#include "Lumiere/ResourcesManager.h"
#include "Lumiere/Graphics/MaterialPBR.h"

namespace lum::utils
{
gfx::MeshPtr MeshLoader::LoadMeshFromFile(const std::string &filename)
{
    // if we cached the mesh already, just return it
    gfx::MeshPtr cachedMesh = ResourcesManager::Instance()->GetMesh(filename);
    if (ResourcesManager::Instance()->GetMesh(filename) != nullptr)
        return cachedMesh;

    // else, load it with assimp
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filename.c_str(),
        aiProcess_JoinIdenticalVertices   |
        aiProcess_SortByPType             |
        aiProcess_GenNormals              | // create the normals if not already in the file
        aiProcess_CalcTangentSpace          // create tan/bitan
        );

    if (scene == nullptr)
    {
        std::cerr << "Couldn't load the mesh at " << filename << "\nWith error:" << importer.GetErrorString() << "\n";
        return {};
    }

    directory = filename.substr(0, filename.find_last_of('/'));
    std::vector<gfx::SubMesh> meshes;
    ProcessNode(scene->mRootNode, scene, meshes);

    std::cout << "Loaded mesh " << filename << "\n";
    return ResourcesManager::Instance()->CacheMesh(filename, meshes);
}

void MeshLoader::ProcessNode(aiNode *node, const aiScene *scene, std::vector<gfx::SubMesh> &subMeshes)
{
    for (uint32_t i = 0; i < node->mNumMeshes; i++)
    {
         subMeshes.push_back(ProcessMesh(scene->mMeshes[node->mMeshes[i]], scene));
    }
    for (uint32_t i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, subMeshes);
    }
}

gfx::SubMesh MeshLoader::ProcessMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<gfx::VertexData> vertices;
    std::vector<uint32_t> indices;

    for (int i = 0; i < mesh->mNumVertices; i++)
    {
        gfx::VertexData vertex;
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
    gpu::TexturePtr albedo;
    // we assume that for PBR materials, we only use one texture per channel.
    aiString texPath;
    if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == aiReturn_SUCCESS)
    {
        // try to retrieve the texture, and create it if it was not cached
        const std::string fullPath = directory + "/" + texPath.C_Str();
        albedo = ResourcesManager::Instance()->GetTexture(std::hash<std::string>()(fullPath));
        if (albedo == nullptr)
            albedo = ResourcesManager::Instance()->CacheTexture(gpu::Texture::TextureTarget::Target2D, fullPath, true);
    }

    gpu::ShaderPtr   pbrShader = lum::ResourcesManager::Instance()->GetShader(lum::ResourcesManager::PBR_LIT_SHADER_KEY);
    gfx::MaterialPtr pbrMat    = std::make_shared<gfx::MaterialPBR>(pbrShader, albedo, nullptr, nullptr, nullptr);

    return {vertices, indices, pbrMat};
}
} // lum