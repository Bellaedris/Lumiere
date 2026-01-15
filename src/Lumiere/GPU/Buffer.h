//
// Created by Bellaedris on 12/12/2025.
//

#pragma once

#include <cstdint>

#include <glad/glad.h>
#include "GLUtils.h"

namespace lum::gpu
{
/**
 * \brief A wrapper around an OpenGL Buffer.
 */
class Buffer
{
    #pragma region Enum
public:
    /**
     * \brief What we are storing in a buffer: vertices, indices
     */
    enum BufferType
    {
        Vertex,
        Index,
        Uniform
    };

    /**
     * \brief Hints for the driver on how the buffer data will be used. Most of the time, we use static draw: we set
     * the data once, and use it a lot (every frame).
     * Detailed infos here: https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBufferData.xhtml
     */
    enum BufferUsage
    {
        STATIC_DRAW,
        STATIC_READ,
        STATIC_COPY,
        DYNAMIC_DRAW,
        DYNAMIC_READ,
        DYNAMIC_COPY,
        STREAM_DRAW,
        STREAM_READ,
        STREAM_COPY
    };
    #pragma endregion Enum

private:
    #pragma region Members
    uint32_t m_buffer{0};
    BufferType m_type;
    #pragma endregion Members

    #pragma region Static Methods
    /**
     * \brief Maps a buffer type to the corresponding OpenGL enum
     * \param type The type of a buffer
     * \return the OpenGL enum
     */
    static GLuint GetBufferType(BufferType type);

    /**
     * \brief Maps a buffer usage to its corresponding OpenGL enum
     * \param usage the usage of a buffer
     * \return the OpenGL enum
     */
    static GLuint GetBufferUsage(BufferUsage usage);
    #pragma endregion Static Methods
public:
    Buffer(BufferType type);
    ~Buffer();

    // same than VAOs, Buffers aren't copy-constructible, they only implement move semantics.
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    Buffer(Buffer&& other) noexcept;
    Buffer& operator=(Buffer&& other) noexcept;

    #pragma region Methods
    /**
     * \brief Binds the buffer to be able to use it. Must be called before any operation on the buffer
     */
    void Bind() const;

    /**
     * \brief binds the buffer to an indexed target. Only available for Uniform Buffers.
     */
    void Bind(uint32_t index) const;

    /**
     * \brief Populate a buffer with data. The buffer must be bound beforehand using the Bind() method
     * \param size Size of the data to allocate
     * \param data pointer to the data to allocate
     * \param usage How the data will be used. This is a hint and only helps the driver speedup things.
     * See BufferUsage for more detailed informations.
     */
    void Write(uint32_t size, void* data, BufferUsage usage);
    #pragma endregion Methods
};
} // mgl::gpu