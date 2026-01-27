//
// Created by Bellaedris on 16/12/2025.
//

#pragma once

#include <array>
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
    /** \brief a framebuffer can hold up to 8 color attachments */
    std::array<bool, 8> m_attachmentsState {};
    std::vector<uint32_t> m_usedAttachments {};
    #pragma endregion Members
public:
    ~Framebuffer();
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer& other) = delete;
    Framebuffer(Framebuffer&& other) noexcept;
    Framebuffer& operator=(Framebuffer&& other) noexcept;

    Framebuffer(uint32_t width, uint32_t height);

    void SetSize(uint32_t width, uint32_t height) { m_width = width; m_height = height; };
    [[nodiscard]] uint32_t Handle() const { return m_handle; };

    /**
     * \brief Attach a texture to a framebuffer. Can be used to add depth, stencil or multiple color attachments
     * \param attachment The type of attachment to bind to this framebuffer
     * \param texture the texture to attach
     * \param colorAttachmentId Optional. Only useful for color attachment, tell which color attachment will be bound,
     * more on that here https://registry.khronos.org/OpenGL-Refpages/gl4/html/glFramebufferTexture.xhtml
     */
    void Attach(Attachment attachment, const TexturePtr& texture, int colorAttachmentId = 0);
    void Bind(Type type) const;
    void Unbind(Type type) const;

    #pragma region StaticMethods
    static void BlitFramebuffer(const std::unique_ptr<Framebuffer>& src, const std::unique_ptr<Framebuffer>& dst, int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1, Attachment whatToCopy, Texture::Filtering filter);
    #pragma endregion StaticMethods
};
}