//
// Created by Bellaedris on 12/12/2025.
//

#pragma once

#include <glad/glad.h>
#include <stdexcept>
#include <iostream>

#include <glm/glm.hpp>

namespace lum::gpu
{
    /**
     * \brief Contains any enum/constant not linked to a single type of OpenGL object, and the associated functions.
     */
    class GLUtils
    {
    public:
        /**
         * \brief Wrapper on the OpenGL types. Functions like VertexAttribPointer will ask for a type, but it is cumbersome
         * to open Khronos' documentation to look for the correct type, while this enum should be more accessible.
         */
        enum DataType
        {
            Byte,
            UnsignedByte,
            Short,
            UnsignedShort,
            Int,
            UnsignedInt,
            UnsignedInt_24_8,
            Float,
            Double,
        };

        static GLuint GetDataType(DataType type)
        {
            switch(type)
            {
                case DataType::Float:
                    return GL_FLOAT;
                case DataType::Byte:
                    return GL_BYTE;
                case DataType::UnsignedByte:
                    return GL_UNSIGNED_BYTE;
                case DataType::Short:
                    return GL_SHORT;
                case DataType::UnsignedShort:
                    return GL_UNSIGNED_SHORT;
                case DataType::Int:
                    return GL_INT;
                case DataType::UnsignedInt:
                    return GL_UNSIGNED_INT;
                case UnsignedInt_24_8:
                    return GL_UNSIGNED_INT_24_8;
                case DataType::Double:
                    return GL_DOUBLE;
                default:
                    std::cerr << "Invalid data type";
                    return GL_FALSE;
            }
        }

        enum Access
        {
            Read,
            Write,
            ReadWrite
        };

        static GLint GetAccess(Access access)
        {
            switch (access)
            {
                case Read:
                    return GL_READ_ONLY;
                case Write:
                    return GL_WRITE_ONLY;
                case ReadWrite:
                    return GL_READ_WRITE;
                default:
                    std::cerr << "Invalid access type\n";
                    return GL_FALSE;
            }
        }

        static void ClearColor(const glm::vec4& color)
        {
            glClearColor(color.r, color.g, color.b, color.a);
        }

        /**
         * \brief Clears the currently bound framebuffer
         */
        static void Clear()
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        /**
         * \brief Enables or disables Depth testing
         * \param value true to enable, false to disable
         */
        static void SetDepthTesting(bool value)
        {
            if(value)
                glEnable(GL_DEPTH_TEST);
            else
                glDisable(GL_DEPTH_TEST);
        }

        /**
         * \brief Changes OpenGL's render size
         * \param width width of the new size
         * \param height height of the new size
         */
        static void SetViewportSize(int width, int height)
        {
            glViewport(0, 0, width, height);
        }
    };
}