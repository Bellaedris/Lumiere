//
// Created by Bellaedris on 12/02/2026.
//

#include "ColorAdjustments.h"

#include "ChromaticAberration.h"
#include "Lumiere/ResourcesManager.h"

namespace lum::rdr
{
ColorAdjustments::ColorAdjustments(uint32_t width, uint32_t height)
    : m_width(width)
    , m_height(height)
{
    std::vector<gpu::Shader::ShaderSource> shaderSources =
    {
        {gpu::Shader::ShaderType::Compute, "shaders/colorAdjustments.comp"}
    };
    ResourcesManager::Instance()->CacheShader(COLOR_ADJUSTMENTS_SHADER_NAME, shaderSources);

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
    ResourcesManager::Instance()->CreateTexture(COLOR_ADJUSTMENTS_NAME, output);
}

void ColorAdjustments::Render(const FrameData &frameData)
{
    if (frameData.profilerGPU)
        frameData.profilerGPU->BeginScope("Color Adjustments");

    gpu::ShaderPtr colorShader = ResourcesManager::Instance()->GetShader(COLOR_ADJUSTMENTS_SHADER_NAME);
    colorShader->Bind();
    colorShader->UniformData("brightness", m_postExposure);
    colorShader->UniformData("contrast", m_contrast + 1.f);
    colorShader->UniformData("saturation", m_saturation + 1.f);

    gpu::TexturePtr in = ResourcesManager::Instance()->GetTexture(ChromaticAberration::CHROMATIC_ABERRATION_NAME);
    in->Bind(0);

    gpu::TexturePtr output = ResourcesManager::Instance()->GetTexture(COLOR_ADJUSTMENTS_NAME);
    output->BindImage(1, 0, gpu::GLUtils::Write);

    colorShader->Dispatch(std::ceil(m_width / 16) + 1, std::ceil(m_height / 16) + 1, 1);
    colorShader->Wait();

    if (frameData.profilerGPU)
        frameData.profilerGPU->EndScope("Color Adjustments");
}

void ColorAdjustments::RenderUI()
{
    if (ImGui::TreeNode("Color Adjustments"))
    {
        ImGui::SliderFloat("Post exposure", &m_postExposure, 0.0f, 10.f);
        ImGui::SliderFloat("Contrast", &m_contrast, -1.f, 1.f);
        ImGui::SliderFloat("Saturation", &m_saturation, -1.f, 1.f);
        ImGui::TreePop();
    }
}

void ColorAdjustments::Rebuild(uint32_t width, uint32_t height)
{
    m_width = width;
    m_height = height;

    gpu::TexturePtr output = ResourcesManager::Instance()->GetTexture(COLOR_ADJUSTMENTS_NAME);
    output->SetSize(static_cast<int>(width), static_cast<int>(height));
    output->Reallocate();
}
}
