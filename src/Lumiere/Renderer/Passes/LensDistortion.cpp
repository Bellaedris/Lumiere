//
// Created by Bellaedris on 06/02/2026.
//

#include "LensDistortion.h"

#include "ColorAdjustments.h"
#include "Tonemap.h"
#include "Lumiere/ResourcesManager.h"
#include "Lumiere/Renderer/RenderPipeline.h"

namespace  lum::rdr
{
LensDistortion::LensDistortion(uint32_t width, uint32_t height)
    : m_width(width)
    , m_height(height)
{
    std::vector<gpu::Shader::ShaderSource> shaderSources =
    {
        {gpu::Shader::ShaderType::Compute, "shaders/lensDistortion.comp"}
    };
    ResourcesManager::Instance()->CacheShader(LENS_DISTORTION_SHADER_NAME, shaderSources);

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
    ResourcesManager::Instance()->CreateTexture(LENS_DISTORTION_NAME, output);
}

void LensDistortion::Render(const FrameData &frameData)
{
    if (frameData.profilerGPU)
        frameData.profilerGPU->BeginScope("Lens Distortion");

    gpu::ShaderPtr distortionShader = ResourcesManager::Instance()->GetShader(LENS_DISTORTION_SHADER_NAME);
    distortionShader->Bind();
    distortionShader->UniformData("distortionTightness", m_distortionTightness);
    distortionShader->UniformData("distortionIntensity", m_distortionIntensity);

    gpu::TexturePtr input = ResourcesManager::Instance()->GetTexture(ColorAdjustments::COLOR_ADJUSTMENTS_NAME);
    input->Bind(0);

    gpu::TexturePtr output = ResourcesManager::Instance()->GetTexture(LENS_DISTORTION_NAME);
    output->BindImage(1, 0, gpu::GLUtils::Write);

    distortionShader->Dispatch(std::ceil(m_width / 16) + 1, std::ceil(m_height / 16) + 1, 1);
    distortionShader->Wait();

    if (frameData.profilerGPU)
        frameData.profilerGPU->EndScope("Lens Distortion");
}

void LensDistortion::RenderUI()
{
    if (ImGui::TreeNode("Lens Distortion"))
    {
        ImGui::SliderFloat("Tightness", &m_distortionTightness, 1.f, 16.f);
        ImGui::SliderFloat("Intensity", &m_distortionIntensity, 0.f, 10.f);
        ImGui::TreePop();
    }
}

void LensDistortion::Rebuild(uint32_t width, uint32_t height)
{
    m_width = width;
    m_height = height;

    gpu::TexturePtr output = ResourcesManager::Instance()->GetTexture(LENS_DISTORTION_NAME);
    output->SetSize(static_cast<int>(width), static_cast<int>(height));
    output->Reallocate();
}
}
