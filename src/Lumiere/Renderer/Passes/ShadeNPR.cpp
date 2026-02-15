//
// Created by Bellaedris on 22/01/2026.
//

#include "ShadeNPR.h"

#include "GBuffer.h"
#include "Outline.h"
#include "Lumiere/ResourcesManager.h"
#include "Lumiere/Renderer/RenderPipeline.h"

namespace lum::rdr
{
ShadeNPR::ShadeNPR(uint32_t width, uint32_t height)
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

    gpu::TexturePtr albedo = ResourcesManager::Instance()->CreateTexture(SHADE_NPR_NAME, rgbDesc);
    m_framebuffer->Attach(gpu::Framebuffer::Color, albedo, 0);

    std::vector<gpu::Shader::ShaderSource> sources = {
        {gpu::Shader::Vertex, "shaders/shadeNPR.vert"},
        {gpu::Shader::Fragment, "shaders/shadeNPR.frag"}
    };
    ResourcesManager::Instance()->CacheShader(SHADE_NPR_SHADER_NAME, sources);

    ResourcesManager::Instance()->CacheTexture(gpu::Texture::Target2D, SHADE_NPR_PENCIL_SHADOW_TEXTURE_PATH, true);
}

void ShadeNPR::Render(const FrameData &frameData)
{
    if (frameData.profilerGPU)
        frameData.profilerGPU->BeginScope("ShadeNPR");

    m_framebuffer->Bind(gpu::Framebuffer::ReadWrite);
    gpu::GLUtils::Clear();

    gpu::ShaderPtr shader = ResourcesManager::Instance()->GetShader(SHADE_NPR_SHADER_NAME);
    shader->Bind();

    // send light counts, as we will actively need them
    shader->UniformData("pointLightCount", frameData.scene->Lights()->PointLightCount());
    shader->UniformData("dirLightCount", frameData.scene->Lights()->DirectionalLightCount());

    gpu::TexturePtr gbufferAlbedo = ResourcesManager::Instance()->GetTexture(GBuffer::GBUFFER_ALBEDO_NAME);
    gbufferAlbedo->Bind(0);
    shader->UniformData("GBufferAlbedo", 0);

    gpu::TexturePtr gbufferNormals = ResourcesManager::Instance()->GetTexture(GBuffer::GBUFFER_NORMALS_NAME);
    gbufferNormals->Bind(1);
    shader->UniformData("GBufferNormals", 1);

    gpu::TexturePtr gbufferPositions = ResourcesManager::Instance()->GetTexture(GBuffer::GBUFFER_POSITIONS_NAME);
    gbufferPositions->Bind(2);
    shader->UniformData("GBufferPositions", 2);

    gpu::TexturePtr gbufferDepth = ResourcesManager::Instance()->GetTexture(GBuffer::GBUFFER_DEPTH_NAME);
    gbufferDepth->Bind(3);
    shader->UniformData("GBufferDepth", 3);

    gpu::TexturePtr pencilShadows = ResourcesManager::Instance()->GetTexture(SHADE_NPR_PENCIL_SHADOW_TEXTURE_PATH);
    pencilShadows->Bind(4);
    shader->UniformData("PencilShadows", 4);

    ResourcesManager::Instance()->GetMesh(ResourcesManager::DEFAULT_PLANE_NAME)->Draw();

    m_framebuffer->Unbind(gpu::Framebuffer::ReadWrite);

    if (frameData.profilerGPU)
        frameData.profilerGPU->EndScope("ShadeNPR");
}

void ShadeNPR::RenderUI()
{
   if (ImGui::TreeNode("ShadeNPR"))
   {
       if (ImGui::CollapsingHeader("Preview"))
       {
           IMGUI_PASS_DEBUG_IMAGE_OPENGL(SHADE_NPR_NAME);
       }
       // Eventually add posterization options
       ImGui::TreePop();
   }
}

void ShadeNPR::Rebuild(uint32_t width, uint32_t height)
{
    m_framebuffer->SetSize(width, height);
    gpu::TexturePtr frame = ResourcesManager::Instance()->GetTexture(SHADE_NPR_NAME);
    frame->SetSize(static_cast<int>(width), static_cast<int>(height));
    frame->Reallocate();
}
} // lum::rdr