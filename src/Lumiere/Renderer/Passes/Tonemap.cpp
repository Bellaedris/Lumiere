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
Tonemap::Tonemap(uint32_t width, uint32_t height)
    : m_width(width)
    , m_height(height)
{
    std::vector<gpu::Shader::ShaderSource> shaderSources =
    {
        {gpu::Shader::ShaderType::Compute, "shaders/tonemap.comp"}
    };
    ResourcesManager::Instance()->CacheShader(TONEMAP_SHADER_NAME, shaderSources);

    gpu::Texture::TextureDesc output
    {
        .target = gpu::Texture::Target2D,
        .width = static_cast<int>(width),
        .height = static_cast<int>(height),
        .format = gpu::Texture::RGBA,
        .dataType = gpu::GLUtils::UnsignedByte,
        .minFilter = gpu::Texture::Nearest,
        .magFilter = gpu::Texture::Nearest,
        .wrapMode = gpu::Texture::WrapMode::ClampToBorder
    };
    ResourcesManager::Instance()->CreateTexture(TONEMAP_NAME, output);
}

void Tonemap::Render(const SceneDesc &scene)
{
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
} // lum::rdr