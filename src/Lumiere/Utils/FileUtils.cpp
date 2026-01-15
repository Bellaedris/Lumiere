//
// Created by Bellaedris on 12/12/2025.
//

#include <fstream>
#include <sstream>
#include "FileUtils.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "Lumiere/Mesh.h"

namespace lum::utils
{
std::optional<std::string> FileUtils::read_file(const char *filename)
{
    std::ifstream file(filename, std::ifstream::in);
    if (file)
    {
        //read the shader file
        std::stringbuf s;
        file.get(s, 0);
        file.close();

        return s.str();
    }

    return {};
}

std::vector<MeshPtr> FileUtils::LoadMeshFromFile(const std::string filename)
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

    std::vector<MeshPtr> meshes;
    ProcessNode(scene->mRootNode, scene, meshes);

    return meshes;
}

void FileUtils::ProcessNode(aiNode *node, const aiScene *scene, std::vector<MeshPtr> &meshes)
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

MeshPtr FileUtils::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Mesh::VertexData> vertices;
    std::vector<uint32_t> indices;

    for (int i = 0; i < mesh->mNumVertices; i++)
    {
        Mesh::VertexData vertex;
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

    return std::make_unique<Mesh>(vertices, indices);
}
} // mgl::utils