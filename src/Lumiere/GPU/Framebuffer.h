//
// Created by Bellaedris on 16/12/2025.
//

#pragma once

#include <cstdint>

#include "Texture.h"

namespace lum::gpu
{
class Framebuffer
{
public:
    #pragma region Enums
        enum Type
        {
            Read,
            Write,
            ReadWrite
        };

        enum Attachment
        {
            Color   = 1 << 0,
            Depth   = 1 << 1,
            Stencil = 1 << 2
        };

        static GLint GetType(Type type);
        static GLint GetAttachment(Attachment attachment);
    #pragma endregion
private:
    #pragma region Members
    uint32_t m_handle;

    uint32_t m_width;
    uint32_t m_height;
    std::unique_ptr<Texture> m_colorTexture {nullptr};
    std::unique_ptr<Texture> m_depthTexture {nullptr};
    #pragma endregion Members
public:
    ~Framebuffer();
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer& other) = delete;
    Framebuffer(Framebuffer&& other) noexcept;
    Framebuffer& operator=(Framebuffer&& other) noexcept;

    Framebuffer(uint32_t width, uint32_t height);

    [[nodiscard]] uint32_t Handle() { return m_handle; };
    [[nodiscard]] const std::unique_ptr<Texture>& ColorTexture() const;
    [[nodiscard]] const std::unique_ptr<Texture>& DepthTexture() const;
    void Attach(Attachment attachment);
    void Bind(Type type);
    void Unbind(Type type);

    #pragma region StaticMethods
    static void BlitFramebuffer(const std::unique_ptr<Framebuffer>& src, const std::unique_ptr<Framebuffer>& dst, int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1, Attachment whatToCopy, Texture::Filtering filter);
    #pragma endregion StaticMethods
};
}