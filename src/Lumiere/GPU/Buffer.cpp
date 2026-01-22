//
// Created by Bellaedris on 12/12/2025.
//

#include "Buffer.h"

namespace lum::gpu
{
    GLuint Buffer::GetBufferType(Buffer::BufferType type)
    {
        switch(type)
        {
            case BufferType::Vertex:
                return GL_ARRAY_BUFFER;
            case BufferType::Index:
                return GL_ELEMENT_ARRAY_BUFFER;
            case Uniform:
                return GL_UNIFORM_BUFFER;
            default:
                std::cerr << "Invalid buffer type";
                return GL_FALSE;
        }
    }

    GLuint Buffer::GetBufferUsage(BufferUsage usage)
    {
        switch(usage)
        {
            case BufferUsage::StaticDraw:
                return GL_STATIC_DRAW;
            case BufferUsage::StaticCopy:
                return GL_STATIC_COPY;
            case BufferUsage::StaticRead:
                return GL_STATIC_READ;
            case BufferUsage::DynamicDraw:
                return GL_DYNAMIC_DRAW;
            case BufferUsage::DynamicCopy:
                return GL_DYNAMIC_COPY;
            case BufferUsage::DynamicRead:
                return GL_DYNAMIC_READ;
            case BufferUsage::StreamDraw:
                return GL_STREAM_DRAW;
            case BufferUsage::StreamCopy:
                return GL_STREAM_COPY;
            case BufferUsage::StreamRead:
                return GL_STREAM_READ;
            default:
                std::cerr << "Invalid buffer usage";
                return GL_FALSE;
        }
    }

    Buffer::Buffer(BufferType type)
        : m_type(type)
    {
        glGenBuffers(1, &m_buffer);
    }

    Buffer::~Buffer()
    {
        glDeleteBuffers(1, &m_buffer);
    }

    Buffer::Buffer(Buffer &&other) noexcept
        : m_type(other.m_type)
        , m_buffer(other.m_buffer)
    {
        other.m_buffer = 0;
    }

    Buffer &Buffer::operator=(Buffer &&other) noexcept
    {
        if (this != &other)
        {
            glDeleteBuffers(1, &m_buffer);
            m_buffer       = other.m_buffer;
            m_type         = other.m_type;
            other.m_buffer = 0;
        }
        return *this;
    }

    void Buffer::Bind() const
    {
        if (m_type == Uniform)
        {
            std::cerr << "Uniform Buffers must use the Bound(index) variant method, as they need to be bound to an indexed target.\n";
            return;
        }
        glBindBuffer(GetBufferType(m_type), m_buffer);
    }

    void Buffer::Bind(uint32_t index) const
    {
        if (m_type != Uniform)
        {
            std::cerr << "Only Uniform Buffers can be bound to an indexed target.\n";
            return;
        }

        glBindBufferBase(GetBufferType(m_type), index, m_buffer);
    }

    void Buffer::Write(uint32_t size, void *data, BufferUsage usage)
    {
        glNamedBufferData(m_buffer, size, data, GetBufferUsage(usage));
    }
} // mgl::gpu