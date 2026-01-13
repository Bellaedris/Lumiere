//
// Created by Bellaedris on 12/12/2025.
//

#include "VAO.h"

namespace lum::gpu
{
    Vao::Vao()
    {
        glGenVertexArrays(1, &m_vao);
    }

    Vao::Vao(Vao &&other) noexcept
            : m_vao(other.m_vao)
    {
        other.m_vao = 0;
    }

    Vao &Vao::operator=(Vao &&other) noexcept
    {
        if (this != &other)
        {
            glDeleteVertexArrays(1, &m_vao);
            m_vao = other.m_vao;
            other.m_vao = 0;
        }
        return *this;
    }

    void Vao::Bind() const
    {
        glBindVertexArray(m_vao);
    }

    void Vao::Unbind() const
    {
        glBindVertexArray(0);
    }

    void Vao::SetAttribute(int location, GLUtils::DataType type, int offset, int tupleSize, int stride)
    {
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, tupleSize, GLUtils::GetDataType(type), GL_FALSE, stride, (void*)offset);
    }

    Vao::~Vao()
    {
        glDeleteVertexArrays(1, &m_vao);
    }
} // mgl::gpu