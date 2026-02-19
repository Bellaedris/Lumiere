//
// Created by Bellaedris on 06/02/2026.
//

#include "ChromaticAberration.h"

#include "Tonemap.h"
#include "Lumiere/ResourcesManager.h"
#include "Lumiere/Renderer/RenderPipeline.h"

namespace  lum::rdr
{
REGISTER_TO_PASS_FACTORY(ChromaticAberration, ChromaticAberration::CHROMATIC_ABERRATION_NAME);

ChromaticAberration::ChromaticAberration(uint32_t width, uint32_t height)
    : m_width(width)
    , m_height(height)
{
    ChromaticAberration::Init();
}

void ChromaticAberration::Init()
{
    std::vector<gpu::Shader::ShaderSource> shaderSources =
    {
        {gpu::Shader::ShaderType::Compute, "shaders/chromaticAberration.comp"}
    };
    ResourcesManager::Instance()->CacheShader(CHROMATIC_ABERRATION_SHADER_NAME, shaderSources);

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
    ResourcesManager::Instance()->CreateTexture(CHROMATIC_ABERRATION_NAME, output);
}

void ChromaticAberration::Render(const FrameData &frameData)
{
    if (frameData.profilerGPU)
        frameData.profilerGPU->BeginScope("Chromatic Aberration");

    gpu::ShaderPtr aberrationShader = ResourcesManager::Instance()->GetShader(CHROMATIC_ABERRATION_SHADER_NAME);
    aberrationShader->Bind();
    aberrationShader->UniformData("samples", m_samples);
    aberrationShader->UniformData("intensity", m_intensity);
    aberrationShader->UniformData("exponent", m_exponent);

    gpu::TexturePtr input = ResourcesManager::Instance()->GetTexture(Tonemap::TONEMAP_NAME);
    input->Bind(0);

    gpu::TexturePtr output = ResourcesManager::Instance()->GetTexture(CHROMATIC_ABERRATION_NAME);
    output->BindImage(1, 0, gpu::GLUtils::Write);

    aberrationShader->Dispatch(std::ceil(m_width / 16) + 1, std::ceil(m_height / 16) + 1, 1);
    aberrationShader->Wait();

    if (frameData.profilerGPU)
        frameData.profilerGPU->EndScope("Chromatic Aberration");
}

void ChromaticAberration::RenderUI()
{
    if (ImGui::TreeNode("Chromatic Aberration"))
    {
        ImGui::SliderInt("Samples", &m_samples, 1, 20);
        ImGui::SliderFloat("Intensity", &m_intensity, .0f, 1.f);
        ImGui::SliderFloat("Exponent", &m_exponent, 2, 20);
        ImGui::SetItemTooltip("How much chromatic aberration appears close to the screen center.\nA value closer to 1 will have chromatic aberration falloff linearly when approching the center of the screen, \nwhile a high value will only have aberrations at the very border of the screen.");
        ImGui::TreePop();
    }
}

void ChromaticAberration::Rebuild(uint32_t width, uint32_t height)
{
    m_width = width;
    m_height = height;

    gpu::TexturePtr output = ResourcesManager::Instance()->GetTexture(CHROMATIC_ABERRATION_NAME);
    output->SetSize(static_cast<int>(width), static_cast<int>(height));
    output->Reallocate();
}

void ChromaticAberration::Serialize(YAML::Node passes)
{
    YAML::Node aberration;
    aberration["name"] = CHROMATIC_ABERRATION_NAME;
    aberration["width"] = m_width;
    aberration["height"] = m_height;
    aberration["samples"] = m_samples;
    aberration["intensity"] = m_intensity;
    aberration["exponent"] = m_exponent;
    passes.push_back(aberration);
}

void ChromaticAberration::Deserialize(YAML::Node pass)
{
    m_width = pass["width"].as<uint32_t>();
    m_height = pass["height"].as<uint32_t>();
    m_samples = pass["samples"].as<int>();
    m_intensity = pass["intensity"].as<float>();
    m_exponent = pass["exponent"].as<float>();
    Init();
}
}
