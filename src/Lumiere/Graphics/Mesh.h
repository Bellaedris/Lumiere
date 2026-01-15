//
// Created by Bellaedris on 12/12/2025.
//

#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include "../GPU/Buffer.h"
#include "../GPU/VAO.h"
#include "IMaterial.h"

#include "../VectorUtils.h"

namespace lum::gfx
{
class Mesh
{
public:
    #pragma region structs
    struct VertexData
    {
        glm::vec3 pos {};
        glm::vec3 normal {};
        glm::vec2 texcoord {};
    };

    struct VertexKey {
        int vertexIndex;
        int texcoordIndex;
        int normalIndex;

        bool operator<(const VertexKey& other) const {
            if (vertexIndex  != other.vertexIndex)
                return vertexIndex  < other.vertexIndex;

            if (texcoordIndex != other.texcoordIndex)
                return texcoordIndex < other.texcoordIndex;

            return normalIndex < other.normalIndex;
        }
    };
    #pragma endregion structs

private:
    #pragma region Members
    /* TODO read vertexData, write these data into separate per-component vectors, then rebuild a vertexData array when
     these data change. This will allow a better flexibility and also allow us to efficiently build our buffer when
     read objects have different input datas (for instance, no normals or UVs)
     */
    std::vector<VertexData> m_verticesData;
    std::vector<uint32_t> m_indices;

    gpu::Buffer m_buffer, m_indexBuffer;
    gpu::Vao m_vao;
    MaterialPtr m_material;
    #pragma endregion Members
public:
    Mesh();
    Mesh(std::vector<VertexData>& vertices, std::vector<uint32_t>& indices, const MaterialPtr& material);

    void RecalculateNormals();
    void SetupGPU();

    void Draw() const;
    void DrawUnindexed();

    #pragma region Static helpers
    static Mesh GeneratePlane(float halfSize);
    #pragma endregion Static helpers
};

using MeshPtr = std::shared_ptr<Mesh>;
} // mgl