//
// Created by Bellaedris on 22/01/2026.
//

#include "ShadeNPR.h"

#include "GBuffer.h"
#include "Outline.h"
#include "Lumiere/ResourcesManager.h"

namespace lum::rdr
{
ShadeNPR::ShadeNPR()
{
    std::vector<gpu::Shader::ShaderSource> sources = {
        {gpu::Shader::Vertex, "shaders/shadeNPR.vert"},
        {gpu::Shader::Fragment, "shaders/shadeNPR.frag"}
    };
    ResourcesManager::Instance()->CacheShader(SHADE_NPR_SHADER_NAME, sources);

    ResourcesManager::Instance()->CacheTexture(gpu::Texture::Target2D, SHADE_NPR_PENCIL_SHADOW_TEXTURE_PATH, true);
}

void ShadeNPR::Render(const SceneDesc &scene)
{
    gpu::GLUtils::Clear();

    gpu::ShaderPtr shader = ResourcesManager::Instance()->GetShader(SHADE_NPR_SHADER_NAME);
    shader->Bind();

    // send light counts, as we will actively need them
    shader->UniformData("pointLightCount", scene.Lights()->PointLightCount());
    shader->UniformData("dirLightCount", scene.Lights()->DirectionalLightCount());

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

    gpu::TexturePtr outlines = ResourcesManager::Instance()->GetTexture(Outline::OUTLINE_SOBEL_NAME);
    outlines->Bind(5);
    shader->UniformData("Outlines", 5);

    ResourcesManager::Instance()->GetMesh(ResourcesManager::DEFAULT_PLANE_NAME)->Draw();
}
} // lum::rdr