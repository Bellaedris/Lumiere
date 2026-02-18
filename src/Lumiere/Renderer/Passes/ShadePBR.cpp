//
// Created by Bellaedris on 29/01/2026.
//

#include "ShadePBR.h"

#include "GBuffer.h"
#include "Lumiere/ResourcesManager.h"
#include "Lumiere/Renderer/RenderPipeline.h"

namespace lum::rdr
{
ShadePBR::ShadePBR(uint32_t width, uint32_t height)
    : m_framebuffer(std::make_unique<gpu::Framebuffer>(width, height))
{
    gpu::Texture::TextureDesc floatDesc
    {
        .target = gpu::Texture::Target2D,
        .width = static_cast<int>(width),
        .height = static_cast<int>(height),
        .format = gpu::Texture::RGBA,
        .dataType = gpu::GLUtils::Float,
        .minFilter = gpu::Texture::LinearMipMapLinear,
        .magFilter = gpu::Texture::Linear,
        .wrapMode = gpu::Texture::WrapMode::ClampToBorder
    };

    gpu::TexturePtr shaded = ResourcesManager::Instance()->CreateTexture(SHADE_PBR_NAME, floatDesc);
    m_framebuffer->Attach(gpu::Framebuffer::Color, shaded, 0);

    std::vector<gpu::Shader::ShaderSource> sources = {
        {gpu::Shader::Vertex, "shaders/shadePBR.vert"},
        {gpu::Shader::Fragment, "shaders/shadePBR.frag"}
    };
    ResourcesManager::Instance()->CacheShader(SHADE_PBR_SHADER_NAME, sources);
}

void ShadePBR::Render(const FrameData &frameData)
{
    if (frameData.profilerGPU)
        frameData.profilerGPU->BeginScope("ShadePBR");

    m_framebuffer->Bind(gpu::Framebuffer::ReadWrite);
    if (m_accumulate)
    {
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE);
    }
    else
    {
        gpu::GLUtils::Clear();
    }

    gpu::ShaderPtr shader = ResourcesManager::Instance()->GetShader(SHADE_PBR_SHADER_NAME);
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

    gpu::TexturePtr gbufferMetalRough = ResourcesManager::Instance()->GetTexture(GBuffer::GBUFFER_METAL_ROUGH_NAME);
    gbufferMetalRough->Bind(3);
    shader->UniformData("GBufferMetalRough", 3);

    gpu::TexturePtr gbufferEmissive = ResourcesManager::Instance()->GetTexture(GBuffer::GBUFFER_EMISSIVE_NAME);
    gbufferEmissive->Bind(4);
    shader->UniformData("GBufferEmissive", 4);

    gpu::TexturePtr gbufferDepth = ResourcesManager::Instance()->GetTexture(GBuffer::GBUFFER_DEPTH_NAME);
    gbufferDepth->Bind(5);
    shader->UniformData("GBufferDepth", 5);

    ResourcesManager::Instance()->GetMesh(ResourcesManager::DEFAULT_PLANE_NAME)->Draw();

    m_framebuffer->Unbind(gpu::Framebuffer::ReadWrite);

    glDisable(GL_BLEND);

    if (frameData.profilerGPU)
        frameData.profilerGPU->EndScope("ShadePBR");
}

void ShadePBR::RenderUI()
{
   if (ImGui::TreeNode("ShadePBR"))
   {
       ImGui::Checkbox("Accumulate", &m_accumulate);
       if (ImGui::CollapsingHeader("Preview"))
       {
           IMGUI_PASS_DEBUG_IMAGE_OPENGL(SHADE_PBR_NAME);
       }
       // Eventually add posterization options
       ImGui::TreePop();
   }
}

void ShadePBR::Rebuild(uint32_t width, uint32_t height)
{
    m_framebuffer->SetSize(width, height);
    gpu::TexturePtr frame = ResourcesManager::Instance()->GetTexture(SHADE_PBR_NAME);
    frame->SetSize(static_cast<int>(width), static_cast<int>(height));
    frame->Reallocate();
}
} // lum::rdr