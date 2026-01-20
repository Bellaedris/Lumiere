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
        : m_width(other.m_width)
        , m_height(other.m_height)
        , m_handle(other.m_handle)
        , m_depthTexture(std::move(other.m_depthTexture))
        , m_colorTexture(std::move(other.m_colorTexture))
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
            m_depthTexture = std::move(other.m_depthTexture);
            m_colorTexture = std::move(other.m_colorTexture);

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

    void Framebuffer::Attach(Attachment attachment, int colorAttachmentId)
    {
        Bind(ReadWrite);

        if (attachment == Depth)
        {
            // depth attachment
            m_depthTexture = std::make_unique<Texture>(Texture::Target2D);
            m_depthTexture->SetSize(m_width, m_height);
            m_depthTexture->SetWrapMode(Texture::ClampToBorder);
            m_depthTexture->SetBorderColor({1., 1., 1., 1.});
            m_depthTexture->SetMinFilter(Texture::Linear);
            m_depthTexture->SetMagFilter(Texture::Linear);
            m_depthTexture->Allocate(Texture::PixelFormat::DepthComponent, GLUtils::DataType::Float);
            glNamedFramebufferTexture(m_handle, GL_DEPTH_ATTACHMENT, m_depthTexture->Handle(), 0);
        }
        else
        {
            // color attachment
            m_colorTexture = std::make_unique<Texture>(Texture::Target2D);
            m_colorTexture->SetSize(m_width, m_height);
            // we don't need mipmaps for a framebuffer
            m_colorTexture->SetMinFilter(Texture::Linear);
            m_colorTexture->SetMagFilter(Texture::Linear);
            m_colorTexture->Allocate(Texture::PixelFormat::RGBA, GLUtils::DataType::Float);
            glNamedFramebufferTexture(m_handle, GL_COLOR_ATTACHMENT0 + attachment, m_colorTexture->Handle(), 0);
        }
        Unbind(ReadWrite);
    }

    void Framebuffer::Bind(Type type)
    {
        glBindFramebuffer(GetType(type), m_handle);
        glViewport(0, 0, static_cast<int>(m_width), static_cast<int>(m_height));
    }

    void Framebuffer::Unbind(Type type)
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

    const std::unique_ptr<Texture>& Framebuffer::ColorTexture() const
    {
        if(m_colorTexture == nullptr)
        {
            std::cerr << "This framebuffer has no color attachment\n";
            return nullptr;
        }
        return m_colorTexture;
    }

    const std::unique_ptr<Texture>& Framebuffer::DepthTexture() const
    {
        if(m_depthTexture == nullptr)
        {
            std::cerr << "This framebuffer has no depth attachment\n";
            return nullptr;
        }
        return m_depthTexture;
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
