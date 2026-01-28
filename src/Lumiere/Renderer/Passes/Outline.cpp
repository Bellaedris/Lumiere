//
// Created by Bellaedris on 23/01/2026.
//

#include "Outline.h"

#include "GBuffer.h"
#include "Lumiere/ResourcesManager.h"
#include "Lumiere/Renderer/RenderPipeline.h"

namespace lum::rdr
{
Outline::Outline(uint32_t width, uint32_t height)
    : m_width(width)
    , m_height(height)
{
    std::vector<gpu::Shader::ShaderSource> shaderSources =
    {
        {gpu::Shader::ShaderType::Compute, "shaders/outlineSobel.comp"}
    };
    ResourcesManager::Instance()->CacheShader(OUTLINE_SOBEL_SHADER_NAME, shaderSources);

    gpu::Texture::TextureDesc greyscale
    {
        .target = gpu::Texture::Target2D,
        .width = static_cast<int>(width),
        .height = static_cast<int>(height),
        .format = gpu::Texture::Red,
        .dataType = gpu::GLUtils::Float,
        .minFilter = gpu::Texture::Nearest,
        .magFilter = gpu::Texture::Nearest,
        .wrapMode = gpu::Texture::WrapMode::ClampToBorder
    };
    ResourcesManager::Instance()->CreateTexture(OUTLINE_SOBEL_NAME, greyscale);
}

void Outline::Render(const SceneDesc &scene)
{
    gpu::ShaderPtr outlineCompute = ResourcesManager::Instance()->GetShader(OUTLINE_SOBEL_SHADER_NAME);
    outlineCompute->Bind();
    outlineCompute->UniformData("lineWidth", m_lineWidth);

    gpu::TexturePtr normals = ResourcesManager::Instance()->GetTexture(GBuffer::GBUFFER_NORMALS_NAME);
    normals->BindImage(0, 0, gpu::GLUtils::Read);

    gpu::TexturePtr outlineHorizontal = ResourcesManager::Instance()->GetTexture(OUTLINE_SOBEL_NAME);
    outlineHorizontal->BindImage(1, 0, gpu::GLUtils::Write);

    outlineCompute->Dispatch(std::ceil(m_width / 16), std::ceil(m_height / 16), 1);
    outlineCompute->Wait();
}

void Outline::RenderUI()
{
    if (ImGui::TreeNode("Outline"))
    {
        ImGui::InputFloat("Line Width", &m_lineWidth);
        if (ImGui::CollapsingHeader("Preview"))
        {
            IMGUI_PASS_DEBUG_IMAGE_OPENGL(OUTLINE_SOBEL_NAME);
        }
        ImGui::TreePop();
    }
}

void Outline::Rebuild(uint32_t width, uint32_t height)
{
    m_width = width;
    m_height = height;

    gpu::TexturePtr outline = ResourcesManager::Instance()->GetTexture(OUTLINE_SOBEL_NAME);
    outline->SetSize(static_cast<int>(width), static_cast<int>(height));
    outline->Reallocate();
}
} // lum::rdr