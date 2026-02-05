//
// Created by Bellaedris on 16/12/2025.
//

#pragma once

#include <cstdint>
#include <glad/glad.h>

#include <stb/stb_image.h>

#include "GLUtils.h"

namespace lum::gpu
{
/**
 * \brief Wraps OpenGL 2D textures creation
 */
class Texture
{
public:
    #pragma region Enums
    /**
     * \brief Different texture targets we can bind to. We only handle 2D textures and Cubemaps for now. @see
     */
    enum TextureTarget
    {
        Target2D,
        TargetCubemap
    };

    /**
     * \brief How to handle UVs overflow. See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexParameter.xhtml,
     * section "GL_TEXTURE_WRAP_XXX"
     */
    enum WrapMode
    {
        Repeat,
        RepeatMirror,
        ClampToEdge,
        ClampToBorder
    };

    enum Filtering
    {
        Nearest,
        Linear,
        NearestMipMapNearest,
        NearestMipMapLinear,
        LinearMipMapNearest,
        LinearMipMapLinear
    };

    enum PixelFormat
    {
        DepthComponent,
        DepthStencil,
        Red,
        RG,
        RGB,
        RGBA,
        SRGB
    };
    #pragma endregion Enums

    #pragma region EnumAccessFunctions
    static GLint GetTextureTarget(TextureTarget target);
    static GLint GetWrapMode(WrapMode mode);
    static GLint GetFiltering(Filtering filter);
    // TODO this function is horrible. Replace by a proper gigantic enum with all the formats we will need.
    static GLint GetPixelInternalFormat(PixelFormat format, GLUtils::DataType type);
    static GLint GetPixelFormat(PixelFormat format);
    static GLint GetImageFormat(PixelFormat format);
    #pragma endregion EnumAccessFunctions
    #pragma region Structs
    /**
     * \brief The minimal parameters required to build a texture.
     */
    struct TextureDesc
    {
        TextureTarget target;
        int width;
        int height;
        PixelFormat format;
        GLUtils::DataType dataType;
        Filtering minFilter {LinearMipMapLinear};
        Filtering magFilter {Linear};
        WrapMode wrapMode {WrapMode::Repeat};
    };
    #pragma endregion Structs
private:
    #pragma region Members
    uint32_t m_handle {0};

    int m_width {0};
    int m_height {0};
    TextureTarget m_target;
    WrapMode m_wrappingMethod {WrapMode::Repeat};
    Filtering m_minFilter {Filtering::LinearMipMapLinear};
    Filtering m_magFilter {Filtering::Linear};
    PixelFormat m_format {};
    GLUtils::DataType m_dataType {};
    #pragma endregion Members
public:
    Texture(TextureTarget target);
    Texture(TextureTarget target, const char* path, bool generateMipmaps);
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    [[nodiscard]] inline uint32_t Handle() {return m_handle; };
    void Bind();
    void Bind(uint32_t unit);
    void BindImage(uint32_t unit, uint32_t mipLevel, GLUtils::Access access);
    void Allocate(PixelFormat format, GLUtils::DataType dataType);

    /**
     * \brief Allocate storage for a number of mipmaps. This should be done on allocated textures. Once the mips have been
     * allocated, the texture will be considered final and CANNOT be resized.
     * Useful to build a mip chain.
     * \param levels number of mip levels that we want to allocate. When requesting a level n, it is possible that some mips
     * will have a size < 1. In that case, the function will allocate the maximal number of integer size mips.
     */
    void AllocateMipmapsStorage(int levels);

    /**
     * \brief Allocate the memory again, using the same formats and data types, but to take into account any change
     * that could've happened in between.
     */
    void Reallocate();
    void Write(void* data, PixelFormat format, GLUtils::DataType dataType);

    void SetMinFilter(Filtering filter);
    void SetMagFilter(Filtering filter);
    void SetWrapMode(WrapMode mode);
    void SetBorderColor(const glm::vec4& color);
    void SetSize(int width, int height);

    glm::ivec2 Size() { return {m_width, m_height}; };
};

using TexturePtr = std::shared_ptr<Texture>;
} // mgl