//
// Created by Bellaedris on 20/01/2026.
//

#include "GBuffer.h"

#include "Lumiere/ResourcesManager.h"

namespace lum::rdr
{
GBuffer::GBuffer(uint32_t width, uint32_t height)
    : IPass(width, height)
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

    // send light counts, as we will actively need them
    gbufferShader->UniformData("pointLightCount", scene.Lights()->PointLightCount());
    gbufferShader->UniformData("dirLightCount", scene.Lights()->DirectionalLightCount());

    // scene.ForEach([&](const gfx::SubMesh& primitive, const gfx::MaterialPtr& material)
    // {
    //     material->Bind(gbufferShader);
    //     primitive.Draw();
    // });
    for (const auto& mesh : scene.Meshes())
        for (const auto& submesh : mesh->Primitives())
        {
            submesh.Material()->Bind(gbufferShader);
            submesh.Draw();
        }

    m_framebuffer->Unbind(gpu::Framebuffer::ReadWrite);
}
} // lum::rdr