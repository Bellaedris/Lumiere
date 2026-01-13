//
// Created by Bellaedris on 12/12/2025.
//

#include <iostream>
#include <set>
#include "Mesh.h"

namespace lum
{
    Mesh::Mesh(const char *filename)
        : m_buffer(gpu::Buffer::BufferType::Vertex)
        , m_indexBuffer(gpu::Buffer::BufferType::Index)
    {
        tinyobj::ObjReaderConfig reader_config;

        tinyobj::ObjReader reader;

        if (reader.ParseFromFile(filename, reader_config) == false)
        {
            if (!reader.Error().empty())
            {
                std::cerr << "TinyObjReader Error: " << reader.Error();
            }
            return;
        }

        if (reader.Warning().empty() == false)
        {
            std::cout << "TinyObjReader Warning: " << reader.Warning();
        }

        const tinyobj::attrib_t& attrib = reader.GetAttrib();
        const std::vector<tinyobj::shape_t>& shapes = reader.GetShapes();
        const std::vector<tinyobj::material_t>& materials = reader.GetMaterials();
        std::map<VertexKey, uint32_t> uniqueVertices;

        // Loop over shapes
        for (const auto & shape : shapes)
        {
            // Loop over faces(polygon)
            size_t index_offset = 0;
            for (size_t face = 0; face < shape.mesh.num_face_vertices.size(); face++)
            {
                size_t faceVertices = size_t(shape.mesh.num_face_vertices[face]);

                // Loop over vertices in the face.
                for (size_t v = 0; v < faceVertices; v++)
                {
                    VertexData vertex;
                    // access to vertex
                    tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

                    VertexKey key{ idx.vertex_index, idx.texcoord_index, idx.normal_index };

                    if(uniqueVertices.find(key) == uniqueVertices.end())
                    {
                        tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                        tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                        tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                        vertex.pos = {vx, vy, vz};

                        // Check if `normal_index` is zero or positive. negative = no normal data
                        if (idx.normal_index >= 0)
                        {
                            tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
                            tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
                            tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];

                            vertex.normal = {nx, ny, nz};
                        }

                        // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                        if (idx.texcoord_index >= 0)
                        {
                            tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                            tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];

                            vertex.texcoord = {tx, ty};
                        }

                        uniqueVertices[key] = m_verticesData.size();
                        m_verticesData.push_back(vertex);
                    }
                    m_indices.push_back(uniqueVertices[key]);
                }
                index_offset += faceVertices;

                // per-face material
                // TODO handle material
                // shapes[s].mesh.material_ids[face];
            }
        }

        // fill in GPU datas
        SetupGPU();
    }

    Mesh::Mesh(std::vector<VertexData>& vertices, std::vector<uint32_t>& indices)
        : m_verticesData(std::move(vertices))
        , m_indices(std::move(indices))
        , m_buffer(gpu::Buffer::BufferType::Vertex)
        , m_indexBuffer(gpu::Buffer::BufferType::Index)
    {
        SetupGPU();
    }

    void Mesh::Draw() const
    {
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

        return {vertices, indices};
    }
} // mgl