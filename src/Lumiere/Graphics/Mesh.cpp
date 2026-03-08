//
// Created by Bellaedris on 12/12/2025.
//

#include <iostream>
#include <set>
#include "Mesh.h"

#include <memory>
#include <assimp/Importer.hpp>

namespace lum::gfx
{
SubMesh::SubMesh(std::vector<VertexData>& vertices, std::vector<uint32_t>& indices, const MaterialPtr& material, const std::string& name)
    : m_vertexSize(vertices.size())
    , m_indexSize(indices.size())
    , m_buffer(gpu::Buffer::BufferType::Vertex)
    , m_indexBuffer(gpu::Buffer::BufferType::Index)
    , m_material(material)
    , m_name(name)
{
    m_vao.Bind();
    m_buffer.Bind();
    m_buffer.Write(sizeof(VertexData) * vertices.size(), vertices.data(), gpu::Buffer::BufferUsage::StaticDraw);

    m_indexBuffer.Bind();
    m_indexBuffer.Write(sizeof(uint32_t) * indices.size(), indices.data(), gpu::Buffer::BufferUsage::StaticDraw);

    m_vao.SetAttribute(0, gpu::GLUtils::DataType::Float, 0, 3, sizeof(VertexData)); // positions
    m_vao.SetAttribute(1, gpu::GLUtils::DataType::Float, offsetof(VertexData, normal), 3, sizeof(VertexData)); // normals
    m_vao.SetAttribute(2, gpu::GLUtils::DataType::Float, offsetof(VertexData, normal), 3, sizeof(VertexData)); // tangents
    m_vao.SetAttribute(3, gpu::GLUtils::DataType::Float, offsetof(VertexData, normal), 3, sizeof(VertexData)); // bitangents
    m_vao.SetAttribute(4, gpu::GLUtils::DataType::Float, offsetof(VertexData, texcoord), 2, sizeof(VertexData)); // texcoords

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

std::string Mesh::Name() const
{
    size_t name = m_path.find_last_of('/');
    size_t extension = m_path.find_last_of('.');

    return m_path.substr(name + 1, extension - 1);
}

void Mesh::Draw() const
{
    for (const SubMesh& submesh: m_subMeshes)
    {
        submesh.Draw();
    }
}

std::vector<SubMesh> Mesh::GeneratePlane(float halfSize)
{
    std::vector<VertexData> vertices =
    {
        {{-halfSize, halfSize, 0}, {0, 0, 1}, {1, 0, 0}, {0, 1, 0}, {0.0f, 1.0f}},
        {{-halfSize, -halfSize, 0}, {0, 0, 1}, {1, 0, 0}, {0, 1, 0}, {0.0f, 0.0f}},
        {{halfSize, halfSize, 0}, {0, 0, 1}, {1, 0, 0}, {0, 1, 0}, {1.0f, 1.0f}},
        {{halfSize, -halfSize, 0}, {0, 0, 1}, {1, 0, 0}, {0, 1, 0}, {1.0f, 0.0f}}
    };

    std::vector<uint32_t> indices =
    {
        0, 1, 2,
        1, 3, 2
    };

    std::vector<SubMesh> subMeshes;
    subMeshes.emplace_back(vertices, indices, nullptr, "plane");

    return subMeshes;
}

std::vector<SubMesh> Mesh::GenerateSphere(float radius)
{
    int meridians = 16;
    int parallels = 24;
    std::vector<VertexData> vertices;
    std::vector<uint32_t> indices;

    // add top vertex
    vertices.push_back({
        {0, radius, 0},
        {.0f, 1.f, .0f},
        {1.f, .0f, .0f},
        {.0f, .0f, 1.f},
        {
            (std::atan2(radius, 0) + glm::pi<float>()) * glm::one_over_two_pi<float>(),
            std::acos(0) * glm::one_over_pi<float>()
        }
    });

    float stepMeridian = glm::two_pi<float>() / static_cast<float>(meridians);
    float stepParallel = glm::pi<float>() / static_cast<float>(parallels);
    for (int i = 0; i < parallels - 1; i++)
    {
        float y = std::cos((i + 1) * stepParallel);
        float sinPhi = std::sin((i + 1) * stepParallel);
        for (int j = 0; j < meridians; j++)
        {
            // it's also a normal with norm = 1
            glm::vec3 pos(sinPhi * std::cos(j * stepMeridian), y, sinPhi * std::sin(j * stepMeridian));
            // compute norm/tan/bitan
            glm::vec3 tan;
            if (glm::all(glm::equal(pos, {0, 1, 0})))
                tan = glm::cross(pos, {0, 0, 1});
            else
                tan = glm::cross(pos, {0, 1, 0});
            glm::vec3 bitan = glm::cross(pos, tan);

            // uvs
            float theta = std::acos(pos.y / radius);
            float phi = std::atan2(pos.z, pos.x);
            float u = (phi + glm::pi<float>()) * glm::one_over_two_pi<float>();
            float v = theta * glm::one_over_pi<float>();

            vertices.push_back({
                pos * radius,
                pos,
                tan,
                bitan,
                {u, v}
            });
        }
    }

    // bottom vertex
    vertices.push_back({
        {0, -radius, 0},
        {.0f, -1.f, .0f},
        {1.f, .0f, .0f},
        {.0f, .0f, -1.f},
        {
            (std::atan2(-radius, 0) + glm::pi<float>()) * glm::one_over_two_pi<float>(),
            std::acos(0) * glm::one_over_pi<float>()
        }
    });

    // triangle indices
    // top/bottom
    for (int i = 0; i < meridians; ++i)
    {
        int i0 = i + 1;
        int i1 = (i + 1) % meridians + 1;
        indices.push_back(0);
        indices.push_back(i1);
        indices.push_back(i0);
        i0 = i + meridians * (parallels - 2) + 1;
        i1 = (i + 1) % meridians + meridians * (parallels - 2) + 1;
        indices.push_back(vertices.size() - 1);
        indices.push_back(i0);
        indices.push_back(i1);
    }

    // all the others
    for (int j = 0; j < parallels - 2; j++)
    {
        int j0 = j * meridians + 1;
        int j1 = (j + 1) * meridians + 1;
        for (int i = 0; i < meridians; i++)
        {
            int i0 = j0 + i;
            int i1 = j0 + (i + 1) % meridians;
            int i2 = j1 + (i + 1) % meridians;
            int i3 = j1 + i;
            indices.push_back(i0);
            indices.push_back(i1);
            indices.push_back(i2);

            indices.push_back(i0);
            indices.push_back(i2);
            indices.push_back(i3);
        }
    }

    std::vector<SubMesh> subMeshes;
    subMeshes.emplace_back(vertices, indices, nullptr, "sphere");

    return subMeshes;
}
} // mgl