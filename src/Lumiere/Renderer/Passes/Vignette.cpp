//
// Created by Bellaedris on 06/02/2026.
//

#include "Vignette.h"

#include "Bloom.h"
#include "ChromaticAberration.h"
#include "Lumiere/ResourcesManager.h"
#include "Lumiere/Renderer/RenderPipeline.h"

namespace  lum::rdr
{
Vignette::Vignette(uint32_t width, uint32_t height)
    : m_width(width)
    , m_height(height)
{
    std::vector<gpu::Shader::ShaderSource> shaderSources =
    {
        {gpu::Shader::ShaderType::Compute, "shaders/vignette.comp"}
    };
    ResourcesManager::Instance()->CacheShader(VIGNETTE_SHADER_NAME, shaderSources);

    gpu::Texture::TextureDesc output
    {
        .target = gpu::Texture::Target2D,
        .width = static_cast<int>(width),
        .height = static_cast<int>(height),
        .format = gpu::Texture::RGBA,
        .dataType = gpu::GLUtils::UnsignedByte,
        .minFilter = gpu::Texture::Nearest,
        .magFilter = gpu::Texture::Nearest,
        .wrapMode = gpu::Texture::WrapMode::ClampToEdge
    };
    ResourcesManager::Instance()->CreateTexture(RenderPipeline::RENDERED_FRAME_NAME, output);
}

void Vignette::Render(const SceneDesc &scene)
{
    gpu::ShaderPtr vignetteShader = ResourcesManager::Instance()->GetShader(VIGNETTE_SHADER_NAME);
    vignetteShader->Bind();
    vignetteShader->UniformData("radius", m_radius);

    gpu::TexturePtr in = ResourcesManager::Instance()->GetTexture(ChromaticAberration::CHROMATIC_ABERRATION_NAME);
    in->Bind(0);

    gpu::TexturePtr output = ResourcesManager::Instance()->GetTexture(RenderPipeline::RENDERED_FRAME_NAME);
    output->BindImage(1, 0, gpu::GLUtils::Write);

    vignetteShader->Dispatch(std::ceil(m_width / 16) + 1, std::ceil(m_height / 16) + 1, 1);
    vignetteShader->Wait();
}

void Vignette::RenderUI()
{
    if (ImGui::TreeNode("Vignette"))
    {
        ImGui::SliderFloat("Radius", &m_radius, 0.0f, 2.0f);
        ImGui::TreePop();
    }
}

void Vignette::Rebuild(uint32_t width, uint32_t height)
{
    m_width = width;
    m_height = height;

    gpu::TexturePtr output = ResourcesManager::Instance()->GetTexture(RenderPipeline::RENDERED_FRAME_NAME);
    output->SetSize(static_cast<int>(width), static_cast<int>(height));
    output->Reallocate();
}
}
