//
// Created by Bellaedris on 12/12/2025.
//

#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <utility>
#include "../GPU/Buffer.h"
#include "../GPU/VAO.h"
#include "IMaterial.h"

namespace lum::gfx
{

#pragma region structs
struct VertexData
{
    glm::vec3 pos {};
    glm::vec3 normal {};
    glm::vec3 tangent {};
    glm::vec3 bitangent {};
    glm::vec2 texcoord {};
};
#pragma endregion structs

/**
 * \brief A individual mesh that has vertices data, indices data and is described by a material
 */
class SubMesh
{
private:
    #pragma region Members
    /* TODO read vertexData, write these data into separate per-component vectors, then rebuild a vertexData array when
     these data change. This will allow a better flexibility and also allow us to efficiently build our buffer when
     read objects have different input datas (for instance, no normals or UVs).
     We also want to have our mesh data GPU side only, not CPU, maybe in the future we can have an interface between
     gpu::Mesh and the user that allows for modification (for instance, fetch buffer data from the GPU, edit and reupload,
     without storing a permanent CPU copy
     */

    uint32_t m_vertexSize, m_indexSize;
    gpu::Buffer m_buffer, m_indexBuffer;
    gpu::Vao m_vao;
    MaterialPtr m_material;
    #pragma endregion Members
public:
    SubMesh(std::vector<VertexData>& vertices, std::vector<uint32_t>& indices, const MaterialPtr& material);

    const MaterialPtr& Material() const { return m_material; };

    void Draw() const;
    void DrawUnindexed();
};

class Mesh
{
private:
    std::string m_path;
    std::vector<SubMesh> m_subMeshes {};
public:
    Mesh(std::string path) : m_path {std::move(path)} {};
    Mesh(std::string path, std::vector<SubMesh>& subMeshes) : m_path(std::move(path)), m_subMeshes(std::move(subMeshes)) {}

    void AddSubMesh(SubMesh& submesh) { m_subMeshes.push_back(std::move(submesh)); };
    const SubMesh& GetSubMesh(uint32_t subMeshIndex) const { return m_subMeshes[subMeshIndex]; };
    const std::vector<SubMesh>& Primitives() const { return m_subMeshes; };

    void Draw() const;

    #pragma region Static helpers
    static std::vector<SubMesh> GeneratePlane(float halfSize);
    #pragma endregion Static helpers
};

using MeshPtr = std::shared_ptr<Mesh>;
} // mgl