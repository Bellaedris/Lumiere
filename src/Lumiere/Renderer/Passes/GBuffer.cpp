//
// Created by Bellaedris on 20/01/2026.
//

#include "GBuffer.h"

#include "Lumiere/ResourcesManager.h"
#include "Lumiere/Renderer/RenderPipeline.h"

namespace lum::rdr
{
GBuffer::GBuffer(uint32_t width, uint32_t height)
    : m_framebuffer(std::make_unique<gpu::Framebuffer>(width, height))
{
    // attach to our framebuffer the textures we will render to
    gpu::Texture::TextureDesc albedoDesc
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

    gpu::Texture::TextureDesc floatingDesc
    {
        .target = gpu::Texture::Target2D,
        .width = static_cast<int>(width),
        .height = static_cast<int>(height),
        .format = gpu::Texture::RGBA,
        .dataType = gpu::GLUtils::Float,
        .minFilter = gpu::Texture::Linear,
        .magFilter = gpu::Texture::Linear,
        .wrapMode = gpu::Texture::WrapMode::ClampToBorder
    };

    gpu::Texture::TextureDesc depthDesc
    {
        .target = gpu::Texture::Target2D,
        .width = static_cast<int>(width),
        .height = static_cast<int>(height),
        .format = gpu::Texture::DepthComponent,
        .dataType = gpu::GLUtils::Float,
        .minFilter = gpu::Texture::Linear,
        .magFilter = gpu::Texture::Linear,
        .wrapMode = gpu::Texture::WrapMode::ClampToBorder
    };
    // albedo
    gpu::TexturePtr albedo = ResourcesManager::Instance()->CreateTexture(GBUFFER_ALBEDO_NAME, albedoDesc);
    m_framebuffer->Attach(gpu::Framebuffer::Color, albedo, 0);
    // normals
    gpu::TexturePtr normals = ResourcesManager::Instance()->CreateTexture(GBUFFER_NORMALS_NAME, floatingDesc);
    m_framebuffer->Attach(gpu::Framebuffer::Color, normals, 1);
    // positions
    gpu::TexturePtr positions = ResourcesManager::Instance()->CreateTexture(GBUFFER_POSITIONS_NAME, floatingDesc);
    m_framebuffer->Attach(gpu::Framebuffer::Color, positions, 2);
    // metal roughness
    gpu::TexturePtr metalRough = ResourcesManager::Instance()->CreateTexture(GBUFFER_METAL_ROUGH_NAME, floatingDesc);
    m_framebuffer->Attach(gpu::Framebuffer::Color, metalRough, 3);
    // emissive
    gpu::TexturePtr emissive = ResourcesManager::Instance()->CreateTexture(GBUFFER_EMISSIVE_NAME, floatingDesc);
    m_framebuffer->Attach(gpu::Framebuffer::Color, emissive, 4);
    // depth
    gpu::TexturePtr depth = ResourcesManager::Instance()->CreateTexture(GBUFFER_DEPTH_NAME, depthDesc);
    m_framebuffer->Attach(gpu::Framebuffer::Depth, depth);

    std::vector<gpu::Shader::ShaderSource> sources = {
        {gpu::Shader::Vertex, "shaders/gbuffer.vert"},
        {gpu::Shader::Fragment, "shaders/gbuffer.frag"}
    };
    ResourcesManager::Instance()->CacheShader(GBUFFER_SHADER_NAME, sources);

    m_framebuffer->Bind(gpu::Framebuffer::ReadWrite);
    gpu::GLUtils::ClearColor({.2f, .2f, .2f, 1.f});
    gpu::GLUtils::SetDepthTesting(true);
    m_framebuffer->Unbind(gpu::Framebuffer::ReadWrite);
}

void GBuffer::Render(const SceneDesc &scene)
{
    m_framebuffer->Bind(gpu::Framebuffer::ReadWrite);
    gpu::GLUtils::Clear();
    gpu::ShaderPtr gbufferShader = ResourcesManager::Instance()->GetShader(GBUFFER_SHADER_NAME);
    gbufferShader->Bind();

    glm::mat4 modelMatrix(1.f);
    //modelMatrix = glm::rotate(modelMatrix, glm::radians(90.f), glm::vec3(1, 0, 0));
    gbufferShader->UniformData("modelMatrix", modelMatrix);
    gbufferShader->UniformData("normalMatrix", glm::inverse(glm::transpose(modelMatrix)));

    scene.ForEach([&](const gfx::SubMesh& primitive, const gfx::MaterialPtr& material)
    {
        material->Bind(gbufferShader);
        primitive.Draw();
    });

    m_framebuffer->Unbind(gpu::Framebuffer::ReadWrite);
}

void GBuffer::RenderUI()
{
    if (ImGui::TreeNode("GBuffer"))
    {
        if (ImGui::CollapsingHeader("Preview"))
        {
            ImGui::Text("GBuffer Albedo");
            IMGUI_PASS_DEBUG_IMAGE_OPENGL(GBUFFER_ALBEDO_NAME);

            ImGui::Text("GBuffer Normals");
            IMGUI_PASS_DEBUG_IMAGE_OPENGL(GBUFFER_NORMALS_NAME);

            ImGui::Text("GBuffer Positions");
            IMGUI_PASS_DEBUG_IMAGE_OPENGL(GBUFFER_POSITIONS_NAME);

            ImGui::Text("GBuffer MetalRough");
            IMGUI_PASS_DEBUG_IMAGE_OPENGL(GBUFFER_METAL_ROUGH_NAME);

            ImGui::Text("GBuffer Emissive");
            IMGUI_PASS_DEBUG_IMAGE_OPENGL(GBUFFER_EMISSIVE_NAME);

            ImGui::Text("GBuffer Depth");
            IMGUI_PASS_DEBUG_IMAGE_OPENGL(GBUFFER_DEPTH_NAME);
        }
        ImGui::TreePop();
    }
}

void GBuffer::Rebuild(uint32_t width, uint32_t height)
{
    m_framebuffer->SetSize(width, height);
    int iWidth = static_cast<int>(width);
    int iHeight = static_cast<int>(height);

    // recreate the textures
    gpu::TexturePtr albedo = ResourcesManager::Instance()->GetTexture(GBUFFER_ALBEDO_NAME);
    albedo->SetSize(iWidth, iHeight);
    albedo->Reallocate();
    // normals
    gpu::TexturePtr normals = ResourcesManager::Instance()->GetTexture(GBUFFER_NORMALS_NAME);
    normals->SetSize(iWidth, iHeight);
    normals->Reallocate();
    // positions
    gpu::TexturePtr positions = ResourcesManager::Instance()->GetTexture(GBUFFER_POSITIONS_NAME);
    positions->SetSize(iWidth, iHeight);
    positions->Reallocate();
    // metalRough
    gpu::TexturePtr metalRough = ResourcesManager::Instance()->GetTexture(GBUFFER_METAL_ROUGH_NAME);
    metalRough->SetSize(iWidth, iHeight);
    metalRough->Reallocate();
    // emissive
    gpu::TexturePtr emissive = ResourcesManager::Instance()->GetTexture(GBUFFER_EMISSIVE_NAME);
    emissive->SetSize(iWidth, iHeight);
    emissive->Reallocate();
    // depth
    gpu::TexturePtr depth = ResourcesManager::Instance()->GetTexture(GBUFFER_DEPTH_NAME);
    depth->SetSize(iWidth, iHeight);
    depth->Reallocate();
}
} // lum::rdr