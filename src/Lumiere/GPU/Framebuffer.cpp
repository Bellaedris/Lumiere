//
// Created by Bellaedris on 16/12/2025.
//

#include "Framebuffer.h"

namespace lum::gpu
{
    Framebuffer::~Framebuffer()
    {
        glDeleteFramebuffers(1, &m_handle);
    }

    Framebuffer::Framebuffer(Framebuffer &&other) noexcept
        : m_handle(other.m_handle)
        , m_width(other.m_width)
        , m_height(other.m_height)
    {
        other.m_handle = 0;
    }

    Framebuffer &Framebuffer::operator=(Framebuffer &&other) noexcept
    {
        if (this != &other)
        {
            glDeleteFramebuffers(1, &m_handle);
            m_handle = other.m_handle;
            m_width = other.m_width;
            m_height = other.m_height;

            other.m_handle = 0;
        }
        return *this;
    }

    Framebuffer::Framebuffer(uint32_t width, uint32_t height)
        : m_width(width)
        , m_height(height)
    {
        glGenFramebuffers(1, &m_handle);
    }

void Framebuffer::Attach(Attachment attachment, const TexturePtr& texture, int colorAttachmentId/* = 0 */
    )
    {
        Bind(ReadWrite);

        if (attachment == Depth)
        {
            // depth attachment
           texture->SetWrapMode(Texture::ClampToBorder);
           texture->SetBorderColor({1., 1., 1., 1.});
           texture->SetMinFilter(Texture::Linear);
           texture->SetMagFilter(Texture::Linear);
            glNamedFramebufferTexture(m_handle, GL_DEPTH_ATTACHMENT, texture->Handle(), 0);
        }
        else
        {
            // color attachment
            texture->SetMinFilter(Texture::Linear);
            texture->SetMagFilter(Texture::Linear);
            glNamedFramebufferTexture(m_handle, GL_COLOR_ATTACHMENT0 + colorAttachmentId, texture->Handle(), 0);
            m_attachmentsState[colorAttachmentId] = true;
            // update the active attachments array
            m_usedAttachments.clear();
            for (int i = 0;i < m_attachmentsState.size(); i++)
                if (m_attachmentsState[i])
                    m_usedAttachments.push_back(GL_COLOR_ATTACHMENT0 + i);
            // We must tell OpenGL which color attachment will be used
            glDrawBuffers(m_usedAttachments.size(), m_usedAttachments.data());
        }
        Unbind(ReadWrite);
    }

    void Framebuffer::Bind(Type type) const
    {
        glBindFramebuffer(GetType(type), m_handle);
        glViewport(0, 0, static_cast<int>(m_width), static_cast<int>(m_height));
    }

    void Framebuffer::Unbind(Type type) const
    {
        glBindFramebuffer(GetType(type), 0);
    }

    GLint Framebuffer::GetType(Type type)
    {
        switch (type)
        {
            case Read:
                return GL_READ_FRAMEBUFFER;
            case Write:
                return GL_DRAW_FRAMEBUFFER;
            case ReadWrite:
                return GL_FRAMEBUFFER;
            default:
                std::cerr << "Invalid Framebuffer type\n";
                return GL_FALSE;
        }
    }

    GLint Framebuffer::GetAttachment(Framebuffer::Attachment attachment)
    {
        switch (attachment)
        {
            case Color:
                return GL_COLOR_BUFFER_BIT;
            case Depth:
                return GL_DEPTH_BUFFER_BIT;
            case Stencil:
                return GL_STENCIL_BUFFER_BIT;
            default:
                std::cerr << "Invalid Attachment type\n";
                return GL_FALSE;
        }
    }

    void Framebuffer::BlitFramebuffer(
            const std::unique_ptr<Framebuffer> &src,
            const std::unique_ptr<Framebuffer> &dst,
            int srcX0,
            int srcY0,
            int srcX1,
            int srcY1,
            int dstX0,
            int dstY0,
            int dstX1,
            int dstY1,
            Attachment whatToCopy,
            Texture::Filtering filter
    )
    {
        uint32_t dstHandle = dst == nullptr ? 0 : dst->m_handle;
        glBlitNamedFramebuffer(src->m_handle, dstHandle, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1,
                               GetAttachment(whatToCopy), Texture::GetFiltering(filter));
    }
}
