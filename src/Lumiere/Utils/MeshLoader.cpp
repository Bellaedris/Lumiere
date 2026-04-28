//
// Created by Bellaedris on 15/01/2026.
//

#include "MeshLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "../Graphics/Mesh.h"
#include "Lumiere/ResourcesManager.h"
#include "Lumiere/Graphics/MaterialPBR.h"
#include "Lumiere/Utils/FileUtils.h"

namespace lum::utils
{
gfx::MeshPtr MeshLoader::LoadMeshFromFile(const std::string &filename)
{
    // ensure we normalize the paths we receive to a relative form!
    std::filesystem::path filepath(filename);
    if (filepath.is_absolute())
        filepath = std::filesystem::path(filepath).lexically_relative(cfg::EXECUTABLE_DIR).string();
    std::string path = filepath.string();

    // if we cached the mesh already, read it from the resources
    gfx::MeshPtr cachedMesh = ResourcesManager::Instance()->GetMesh(path);
    if (ResourcesManager::Instance()->GetMesh(path) != nullptr)
        return cachedMesh;

    // else, load it with assimp
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path.c_str(),
        aiProcess_JoinIdenticalVertices   |
        aiProcess_SortByPType             |
        aiProcess_GenNormals              | // create the normals if not already in the file
        aiProcess_CalcTangentSpace        | // create tan/bitan
        aiProcess_FlipUVs
        );

    if (scene == nullptr)
    {
        std::cerr << "Couldn't load the mesh at " << path << "\nWith error:" << importer.GetErrorString() << "\n";
        return {};
    }

    directory = path.substr(0, path.find_last_of('/'));
    loadedObjectName = FileUtils::PathToName(path);

    std::vector<gfx::SubMesh> meshes;
    ProcessNode(scene->mRootNode, scene, meshes);

    std::cout << "Loaded mesh " << path << "\n";
    return ResourcesManager::Instance()->CacheMesh(path, meshes);
}

void MeshLoader::ProcessNode(aiNode *node, const aiScene *scene, std::vector<gfx::SubMesh> &subMeshes)
{
    for (uint32_t i = 0; i < node->mNumMeshes; i++)
    {
         subMeshes.push_back(ProcessMesh(scene->mMeshes[node->mMeshes[i]], scene, i));
    }
    for (uint32_t i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, subMeshes);
    }
}

gfx::SubMesh MeshLoader::ProcessMesh(aiMesh *mesh, const aiScene *scene, int index)
{
    std::string name = mesh->mName.length == 0 ? std::to_string(index) : mesh->mName.C_Str();
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
        vertex.tangent = {mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
        vertex.bitangent = {mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z};

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
    std::string matName;
    if (mat->GetName().length == 0)
        matName = loadedObjectName + "_mat_" + std::to_string(mesh->mMaterialIndex);
    else
        matName = loadedObjectName + "_" + mat->GetName().C_Str();

    // check if the mat is registered already
    std::optional<MaterialRegistryEntry> registeryStatus = ResourcesManager::Instance()->GetRegistryEntry(matName);
    // if not, we register it and load the resource right after
    if (registeryStatus.has_value() == false)
    {
        ResourcesManager::Instance()->RegisterMaterial(matName, true);
        // if the Material is not cached already, create it in the resources manager
        std::optional<gfx::MaterialPtr> cached = ResourcesManager::Instance()->GetMaterial(matName);
        gpu::TexturePtr albedo, normals, metalRough, emissive;
        // we assume that for PBR materials, we only use one texture per channel.
        aiString texPath;
        std::function<std::string(aiTextureType, aiString)> GetTexturePath = [&](aiTextureType type, aiString texPath)
        {
            if (mat->GetTexture(type, 0, &texPath) == aiReturn_SUCCESS)
            {
                // try to retrieve the texture, and create it if it was not cached
                return directory + "/" + texPath.C_Str();
            }
            return std::string("");
        };

        std::string albedoPath = GetTexturePath(aiTextureType_DIFFUSE, texPath);
        std::string normalPath = GetTexturePath(aiTextureType_NORMALS, texPath);
        std::string metalRoughPath = GetTexturePath(aiTextureType_DIFFUSE_ROUGHNESS, texPath);
        std::string emissivePath = GetTexturePath(aiTextureType_EMISSIVE, texPath);

        gfx::MaterialPtr pbrMat = std::make_shared<gfx::MaterialPBR>(matName, albedoPath, normalPath, metalRoughPath, emissivePath);
        ResourcesManager::Instance()->CacheMaterial(pbrMat);
    }
    else
    {
        // if in registry but not loaded, create the material by reading from registry
        if (registeryStatus.value().loaded == false)
        {
            ResourcesManager::Instance()->LoadMaterialFromRegistry(matName);
        }
    }

    return {vertices, indices, matName, name};
}
} // lum