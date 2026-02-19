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
REGISTER_TO_PASS_FACTORY(LensDistortion, LensDistortion::LENS_DISTORTION_NAME)

LensDistortion::LensDistortion(uint32_t width, uint32_t height)
    : m_width(width)
    , m_height(height)
{
    LensDistortion::Init();
}

void LensDistortion::Init()
{
    std::vector<gpu::Shader::ShaderSource> shaderSources =
    {
        {gpu::Shader::ShaderType::Compute, "shaders/lensDistortion.comp"}
    };
    ResourcesManager::Instance()->CacheShader(LENS_DISTORTION_SHADER_NAME, shaderSources);

    gpu::Texture::TextureDesc output
    {
        .target = gpu::Texture::Target2D,
        .width = static_cast<int>(m_width),
        .height = static_cast<int>(m_height),
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

    float amount = 1.6f * std::max(abs(m_distortionIntensity), 1.f);
    float theta = glm::radians(std::min(160.f, amount));
    float sigma = 2.f * std::tan(theta * .5f);

    distortionShader->UniformData
    (
        "distortionCenterScale",
        glm::vec4(m_centerX, m_centerY, std::max(m_intensityX, 1e-4f), std::max(m_intensityY, 1e-4f))
    );
    distortionShader->UniformData
    (
        "distortionAmount",
        glm::vec4(m_distortionIntensity >= .0f ? theta : 1.f / theta, sigma, 1.f / m_screenScale, m_distortionIntensity)
    );

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
        ImGui::SliderFloat("Center X", &m_centerX, -1.f, 1.f);
        ImGui::SliderFloat("Center Y", &m_centerY, -1.f, 1.f);
        ImGui::SliderFloat("Intensity X", &m_intensityX, .0f, 1.f);
        ImGui::SliderFloat("Intensity Y", &m_intensityY, .0f, 1.f);

        ImGui::SetItemTooltip("Global scale factor, zoom in or out so the distortion doesn't leave spilling borders on the corners");
        ImGui::SliderFloat("Screen scaling", &m_screenScale, .01f, 5.f);
        ImGui::SetItemTooltip("Amount of distortion");
        ImGui::SliderFloat("Intensity", &m_distortionIntensity, -100.f, 100.f);
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

void LensDistortion::Serialize(YAML::Node passes)
{
    YAML::Node distortion;
    distortion["name"] = LENS_DISTORTION_NAME;
    distortion["width"] = m_width;
    distortion["height"] = m_height;
    distortion["intensityX"] = m_intensityX;
    distortion["intensityY"] = m_intensityY;
    distortion["centerX"] = m_centerX;
    distortion["centerY"] = m_centerY;
    distortion["distortionIntensity"] = m_distortionIntensity;
    distortion["screenScale"] = m_screenScale;
    passes.push_back(distortion);
}

void LensDistortion::Deserialize(YAML::Node pass)
{
    m_width = pass["width"].as<uint32_t>();
    m_height = pass["height"].as<uint32_t>();
    m_intensityX = pass["intensityX"].as<float>();
    m_intensityY = pass["intensityY"].as<float>();
    m_centerX = pass["centerX"].as<float>();
    m_centerY = pass["centerY"].as<float>();
    m_distortionIntensity = pass["distortionIntensity"].as<float>();
    m_screenScale = pass["screenScale"].as<float>();
    Init();
}
}
