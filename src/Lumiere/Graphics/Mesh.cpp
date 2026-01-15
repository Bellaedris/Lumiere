//
// Created by Bellaedris on 12/12/2025.
//

#include <iostream>
#include <set>
#include "Mesh.h"
#include <assimp/Importer.hpp>

namespace lum::gfx
{
Mesh::Mesh()
    : m_buffer(gpu::Buffer::BufferType::Vertex)
    , m_indexBuffer(gpu::Buffer::BufferType::Index)
{}

Mesh::Mesh(std::vector<VertexData>& vertices, std::vector<uint32_t>& indices, const MaterialPtr& material)
    : m_verticesData(std::move(vertices))
    , m_indices(std::move(indices))
    , m_buffer(gpu::Buffer::BufferType::Vertex)
    , m_indexBuffer(gpu::Buffer::BufferType::Index)
    , m_material(material)
{
    SetupGPU();
}

void Mesh::Draw() const
{
    m_material->Bind();
    m_vao.Bind();
    glDrawElements(GL_TRIANGLES, m_indices.size(), gpu::GLUtils::GetDataType(gpu::GLUtils::DataType::UnsignedInt), nullptr);
    m_vao.Unbind();
}

void Mesh::DrawUnindexed()
{

}

void Mesh::RecalculateNormals()
{
    for(int i = 0; i < m_indices.size(); i++)
    {

    }
}

void Mesh::SetupGPU()
{
    m_vao.Bind();
    m_buffer.Bind();
    m_buffer.Write(sizeof(VertexData) * m_verticesData.size(), m_verticesData.data(), gpu::Buffer::BufferUsage::STATIC_DRAW);

    m_indexBuffer.Bind();
    m_indexBuffer.Write(sizeof(uint32_t) * m_indices.size(), m_indices.data(), gpu::Buffer::BufferUsage::STATIC_DRAW);

    m_vao.SetAttribute(0, gpu::GLUtils::DataType::Float, 0, 3, sizeof(VertexData)); // positions
    m_vao.SetAttribute(1, gpu::GLUtils::DataType::Float, offsetof(VertexData, normal), 3, sizeof(VertexData)); // normals
    m_vao.SetAttribute(2, gpu::GLUtils::DataType::Float, offsetof(VertexData, texcoord), 2, sizeof(VertexData)); // texcoords

    m_vao.Unbind();
}

Mesh Mesh::GeneratePlane(float halfSize)
{
    std::vector<VertexData> vertices =
    {
        {{-halfSize, 0, halfSize}, {0, 1, 0}, {0.0f, 1.0f}},
        {{-halfSize, 0, -halfSize}, {0, 1, 0}, {0.0f, 0.0f}},
        {{halfSize, 0, halfSize}, {0, 1, 0}, {1.0f, 1.0f}},
        {{halfSize, 0, -halfSize}, {0, 1, 0}, {1.0f, 0.0f}}
    };

    std::vector<uint32_t> indices =
    {
        0, 1, 2,
        1, 3, 2
    };

    return {vertices, indices, nullptr};
}
} // mgl