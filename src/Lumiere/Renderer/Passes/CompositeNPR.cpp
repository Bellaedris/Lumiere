//
// Created by Bellaedris on 27/01/2026.
//

#include "CompositeNPR.h"

#include "Outline.h"
#include "ShadeNPR.h"
#include "Lumiere/ResourcesManager.h"
#include "Lumiere/Renderer/RenderPipeline.h"

namespace lum::rdr
{
CompositeNPR::CompositeNPR(uint32_t width, uint32_t height)
    : m_framebuffer(std::make_unique<gpu::Framebuffer>(width, height))
{
    gpu::Texture::TextureDesc rgbDesc
    {
        .target = gpu::Texture::Target2D,
        .width = static_cast<int>(width),
        .height = static_cast<int>(height),
        .format = gpu::Texture::RGBA,
        .dataType = gpu::GLUtils::UnsignedByte,
        .minFilter = gpu::Texture::LinearMipMapLinear,
        .magFilter = gpu::Texture::Linear,
        .wrapMode = gpu::Texture::WrapMode::ClampToBorder
    };

    gpu::TexturePtr albedo = ResourcesManager::Instance()->CreateTexture(RenderPipeline::RENDERED_FRAME_NAME, rgbDesc);
    m_framebuffer->Attach(gpu::Framebuffer::Color, albedo, 0);

    std::vector<gpu::Shader::ShaderSource> sources = {
        {gpu::Shader::Vertex, "shaders/compositeNPR.vert"},
        {gpu::Shader::Fragment, "shaders/compositeNPR.frag"}
    };
    ResourcesManager::Instance()->CacheShader(COMPOSITE_NPR_SHADER_NAME, sources);
}

void CompositeNPR::Render(const SceneDesc &scene)
{
    m_framebuffer->Bind(gpu::Framebuffer::ReadWrite);
    gpu::GLUtils::Clear();

    gpu::ShaderPtr shader = ResourcesManager::Instance()->GetShader(COMPOSITE_NPR_SHADER_NAME);
    shader->Bind();

    gpu::TexturePtr shaded = ResourcesManager::Instance()->GetTexture(ShadeNPR::SHADE_NPR_NAME);
    shaded->Bind(0);
    shader->UniformData("ShadedNPR", 0);

    gpu::TexturePtr outlines = ResourcesManager::Instance()->GetTexture(Outline::OUTLINE_SOBEL_NAME);
    outlines->Bind(5);
    shader->UniformData("Outlines", 5);

    ResourcesManager::Instance()->GetMesh(ResourcesManager::DEFAULT_PLANE_NAME)->Draw();

    m_framebuffer->Unbind(gpu::Framebuffer::ReadWrite);
}

void CompositeNPR::RenderUI()
{
    // this is the final composition stage, nothing to change here.
}

void CompositeNPR::Rebuild(uint32_t width, uint32_t height)
{
    m_framebuffer->SetSize(width, height);
    gpu::TexturePtr frame = ResourcesManager::Instance()->GetTexture(RenderPipeline::RENDERED_FRAME_NAME);
    frame->SetSize(static_cast<int>(width), static_cast<int>(height));
    frame->Reallocate();
}
} // lum::rdr