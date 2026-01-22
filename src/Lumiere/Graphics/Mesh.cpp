//
// Created by Bellaedris on 12/12/2025.
//

#include <iostream>
#include <set>
#include "Mesh.h"
#include <assimp/Importer.hpp>

namespace lum::gfx
{
SubMesh::SubMesh(std::vector<VertexData>& vertices, std::vector<uint32_t>& indices, const MaterialPtr& material)
    : m_vertexSize(vertices.size())
    , m_indexSize(indices.size())
    , m_buffer(gpu::Buffer::BufferType::Vertex)
    , m_indexBuffer(gpu::Buffer::BufferType::Index)
    , m_material(material)
{
    m_vao.Bind();
    m_buffer.Bind();
    m_buffer.Write(sizeof(VertexData) * vertices.size(), vertices.data(), gpu::Buffer::BufferUsage::StaticDraw);

    m_indexBuffer.Bind();
    m_indexBuffer.Write(sizeof(uint32_t) * indices.size(), indices.data(), gpu::Buffer::BufferUsage::StaticDraw);

    m_vao.SetAttribute(0, gpu::GLUtils::DataType::Float, 0, 3, sizeof(VertexData)); // positions
    m_vao.SetAttribute(1, gpu::GLUtils::DataType::Float, offsetof(VertexData, normal), 3, sizeof(VertexData)); // normals
    m_vao.SetAttribute(2, gpu::GLUtils::DataType::Float, offsetof(VertexData, texcoord), 2, sizeof(VertexData)); // texcoords

    m_vao.Unbind();
}

void SubMesh::Draw() const
{
    m_vao.Bind();
    glDrawElements(GL_TRIANGLES, static_cast<int>(m_indexSize), gpu::GLUtils::GetDataType(gpu::GLUtils::DataType::UnsignedInt), nullptr);
    m_vao.Unbind();
}

void SubMesh::DrawUnindexed()
{

}

void Mesh::Draw() const
{
    for (const SubMesh& submesh: m_subMeshes)
    {
        submesh.Draw();
    }
}

Mesh Mesh::GeneratePlane(float halfSize)
{
    Mesh m(DEFAULT_PLANE_NAME);

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

    SubMesh plane = {vertices, indices, nullptr};

    m.AddSubMesh(plane);

    return m;
}
} // mgl