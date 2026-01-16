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
    #pragma region Enums
public:
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
    void Write(void* data, PixelFormat format, GLUtils::DataType dataType);

    void SetMinFilter(Filtering filter);
    void SetMagFilter(Filtering filter);
    void SetWrapMode(WrapMode mode);
    void SetBorderColor(const glm::vec4& color);
    void SetSize(int width, int height);
};

using TexturePtr = std::shared_ptr<Texture>;
} // mgl