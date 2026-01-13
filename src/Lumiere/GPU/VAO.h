//
// Created by Bellaedris on 12/12/2025.
//

#pragma once

#include <cstdint>
#include <glad/glad.h>
#include "GLUtils.h"
#include "Buffer.h"

namespace lum::gpu
{
/**
 * \brief A wrapper around an OpenGL Vertex Array Object, that links attributes to a buffer.
 */
class Vao
{
private:
    uint32_t m_vao{0};

public:
    Vao();
    ~Vao();

    // Since I bundle my VAOs into other classes, i will very likely get copies of these VAOs, for instance when doing
    // push_back(). Even tho we copy the Vao ID, we also call the VAO destructor that deletes the resource, which deletes the
    // GPU representation. We should disallow VAO copies and force move.
    // See https://wikis.khronos.org/opengl/Common_Mistakes#RAII_and_hidden_destructor_calls
    Vao(const Vao&) = delete;
    Vao& operator=(const Vao&) = delete;

    Vao(Vao&& other) noexcept;
    Vao& operator=(Vao&& other) noexcept;

    /**
     * \brief Binds the VAO for usage
     */
    void Bind() const;

    /**
     * \brief Unbinds the VAO
     */
    void Unbind() const;

    /**
     * \brief Create an attribute for this VAO
     * \param location Attribute location, that will be used in a shader
     * \param type Data Type @see GLUtils::DataType
     * \param offset Offset from the beginning of the buffer
     * \param tupleSize The number of elements of each vertex: if we are reading triangles, we read our vertices 3 by 3, so 3.
     * if we were to render lines, it would be 2.
     * \param stride Space between 2 values. Will be non-null if we pack multiple datas.
     * We could for instance choose to pack Mesh datas like so:
     * VERTEX UV NORMAL VERTEX UV NORMAL VERTEX UV NORMAL
     * in which case we will have 3 attributes, each with a different stride: For Vertex it will be sizeof(UV) + sizeof(NORMAL).
     * If we were to pack our data like so:
     * VERTEX VERTEX VERTEX UV UV UV NORMAL NORMAL NORMAL
     * We would have no stride, as all vertices are packed together, but we would have an offset: the Normals attribute would have
     * an offset of sizeof(VERTEX) * vertexCount + sizeof(UV) * vertexCount.
     * The first method has the advantage of faster memory access, since every vertex has all its data contiguous in memory.
     * The second method is conceptually simpler but a bit slower (this shouldn't be very noticeable honestly, but benchmark it!)
     */
    void SetAttribute(int location, GLUtils::DataType type, int offset, int tupleSize, int stride);
};
} // mgl::gpu