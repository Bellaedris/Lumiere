//
// Created by Bellaedris on 30/01/2026.
//

#include "Tonemap.h"

#include "Bloom.h"
#include "DepthOfField.h"
#include "ShadePBR.h"
#include "Vignette.h"
#include "Lumiere/ResourcesManager.h"
#include "Lumiere/Renderer/RenderPipeline.h"

namespace lum::rdr
{
REGISTER_TO_PASS_FACTORY(Tonemap, Tonemap::TONEMAP_NAME)

Tonemap::Tonemap(uint32_t width, uint32_t height)
    : m_width(width)
    , m_height(height)
{
    Tonemap::Init();
}

void Tonemap::Init()
{
    std::vector<gpu::Shader::ShaderSource> shaderSources =
    {
        {gpu::Shader::ShaderType::Compute, "shaders/tonemap.comp"}
    };
    ResourcesManager::Instance()->CacheShader(TONEMAP_SHADER_NAME, shaderSources);

    gpu::Texture::TextureDesc output
    {
        .target = gpu::Texture::Target2D,
        .width = static_cast<int>(m_width),
        .height = static_cast<int>(m_height),
        .format = gpu::Texture::RGBA,
        .dataType = gpu::GLUtils::UnsignedByte,
        .minFilter = gpu::Texture::Nearest,
        .magFilter = gpu::Texture::Nearest,
        .wrapMode = gpu::Texture::WrapMode::ClampToBorder
    };
    ResourcesManager::Instance()->CreateTexture(TONEMAP_NAME, output);
}

void Tonemap::Render(const FrameData &frameData)
{
    if (frameData.profilerGPU)
        frameData.profilerGPU->BeginScope("Tonemap");

    gpu::ShaderPtr tonemapCompute = ResourcesManager::Instance()->GetShader(TONEMAP_SHADER_NAME);
    tonemapCompute->Bind();
    tonemapCompute->UniformData("technique", m_technique);
    tonemapCompute->UniformData("gamma", m_gamma);

    gpu::TexturePtr frame = ResourcesManager::Instance()->GetTexture(DepthOfField::DOF_NAME);
    frame->BindImage(0, 0, gpu::GLUtils::Read);

    gpu::TexturePtr out = ResourcesManager::Instance()->GetTexture(TONEMAP_NAME);
    out->BindImage(1, 0, gpu::GLUtils::Write);

    tonemapCompute->Dispatch(std::ceil(m_width / 16) + 1, std::ceil(m_height / 16) + 1, 1);
    tonemapCompute->Wait();

    if (frameData.profilerGPU)
        frameData.profilerGPU->EndScope("Tonemap");
}

void Tonemap::RenderUI()
{
    if (ImGui::TreeNode("Tonemap"))
    {
        if (ImGui::BeginCombo("Technique", m_techniques[m_technique]))
        {
            for (int i = 0; i < m_techniques.size(); i++)
            {
                bool isSelected = m_technique == i;
                if (ImGui::Selectable(m_techniques[i], isSelected))
                    m_technique = i;

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::InputFloat("Gamma", &m_gamma);
        ImGui::TreePop();
    }
}

void Tonemap::Rebuild(uint32_t width, uint32_t height)
{
    m_width = width;
    m_height = height;

    gpu::TexturePtr outline = ResourcesManager::Instance()->GetTexture(TONEMAP_NAME);
    outline->SetSize(static_cast<int>(width), static_cast<int>(height));
    outline->Reallocate();
}

void Tonemap::Serialize(YAML::Node passes)
{
    YAML::Node tonemap;
    tonemap["name"] = TONEMAP_NAME;
    tonemap["width"] = m_width;
    tonemap["height"] = m_height;
    tonemap["technique"] = m_technique;
    tonemap["gamma"] = m_gamma;
    passes.push_back(tonemap);
}

void Tonemap::Deserialize(YAML::Node pass)
{
    m_width = pass["width"].as<uint32_t>();
    m_height = pass["height"].as<uint32_t>();
    m_technique = pass["technique"].as<int>();
    m_gamma = pass["gamma"].as<float>();
    Init();
}
} // lum::rdr