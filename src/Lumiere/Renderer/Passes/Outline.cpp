//
// Created by Bellaedris on 23/01/2026.
//

#include "Outline.h"

#include "GBuffer.h"
#include "Lumiere/ResourcesManager.h"

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

    gpu::TexturePtr normals = ResourcesManager::Instance()->GetTexture(GBuffer::GBUFFER_NORMALS_NAME);
    normals->BindImage(0, 0, gpu::GLUtils::Read);

    gpu::TexturePtr outlineHorizontal = ResourcesManager::Instance()->GetTexture(OUTLINE_SOBEL_NAME);
    outlineHorizontal->BindImage(1, 0, gpu::GLUtils::Write);

    // glm::vec3 val(1, 0, 0);
    // glClearTexImage(outlineHorizontal->Handle(), 0, GL_RED, GL_FLOAT, glm::value_ptr(val));
    outlineCompute->Dispatch(std::ceil(m_width / 16), std::ceil(m_height / 16), 1);
    outlineCompute->Wait();
}
} // lum::rdr