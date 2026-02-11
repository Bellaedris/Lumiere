//
// Created by Bellaedris on 10/02/2026.
//

#include "DepthOfField.h"

#include "Bloom.h"
#include "GBuffer.h"
#include "Lumiere/ResourcesManager.h"
#include "Lumiere/Renderer/RenderPipeline.h"

namespace lum::rdr
{
DepthOfField::DepthOfField(uint32_t width, uint32_t height)
    : m_width(width)
    , m_height(height)
    , m_intermediateWidth(width / 2)
    , m_intermediateHeight(height / 2)
{
    gpu::Texture::TextureDesc nearFarDesc =
    {
        .target = gpu::Texture::Target2D,
        .width = static_cast<int>(m_intermediateWidth),
        .height = static_cast<int>(m_intermediateHeight),
        .format = gpu::Texture::RG,
        .dataType = gpu::GLUtils::Float,
        .minFilter = gpu::Texture::Linear,
        .magFilter = gpu::Texture::Linear,
        .wrapMode = gpu::Texture::WrapMode::ClampToEdge
    };

    gpu::Texture::TextureDesc blurDesc =
    {
        .target = gpu::Texture::Target2D,
        .width = static_cast<int>(m_intermediateWidth),
        .height = static_cast<int>(m_intermediateHeight),
        .format = gpu::Texture::RGBA,
        .dataType = gpu::GLUtils::Float,
        .minFilter = gpu::Texture::Linear,
        .magFilter = gpu::Texture::Linear,
        .wrapMode = gpu::Texture::WrapMode::ClampToEdge
    };

    gpu::Texture::TextureDesc finalDesc =
    {
        .target = gpu::Texture::Target2D,
        .width = static_cast<int>(m_width),
        .height = static_cast<int>(m_height),
        .format = gpu::Texture::RGBA,
        .dataType = gpu::GLUtils::Float,
        .minFilter = gpu::Texture::Linear,
        .magFilter = gpu::Texture::Linear,
        .wrapMode = gpu::Texture::WrapMode::ClampToEdge
    };

    ResourcesManager::Instance()->CreateTexture(DOF_SEPARATION_NAME, nearFarDesc);
    ResourcesManager::Instance()->CreateTexture(DOF_TEMP_BLUR_NAME, blurDesc);
    ResourcesManager::Instance()->CreateTexture(DOF_NEAR_BLUR_NAME, blurDesc);
    ResourcesManager::Instance()->CreateTexture(DOF_FAR_BLUR_NAME, blurDesc);
    ResourcesManager::Instance()->CreateTexture(DOF_NAME, finalDesc);

    std::vector<gpu::Shader::ShaderSource> sources =
    {
        {gpu::Shader::Compute, "shaders/dof_separate.comp"}
    };
    ResourcesManager::Instance()->CacheShader(DOF_PLANE_SEPARATION_SHADER_NAME, sources);

    std::vector<gpu::Shader::ShaderSource> blurNearSources =
    {
        {gpu::Shader::Compute, "shaders/dof_blur_near.comp"}
    };
    ResourcesManager::Instance()->CacheShader(DOF_NEAR_BLUR_SHADER_NAME, blurNearSources);

    std::vector<gpu::Shader::ShaderSource> blurFarSources =
    {
        {gpu::Shader::Compute, "shaders/dof_blur_far.comp"}
    };
    ResourcesManager::Instance()->CacheShader(DOF_FAR_BLUR_SHADER_NAME, blurFarSources);

    std::vector<gpu::Shader::ShaderSource> compositeSources =
    {
        {gpu::Shader::Compute, "shaders/dof_composite.comp"}
    };
    ResourcesManager::Instance()->CacheShader(DOF_COMPOSITE_SHADER_NAME, compositeSources);
}

void DepthOfField::Render(const SceneDesc &scene)
{
    gpu::ShaderPtr separationShader = ResourcesManager::Instance()->GetShader(DOF_PLANE_SEPARATION_SHADER_NAME);
    separationShader->Bind();
    separationShader->UniformData("focusDistance", m_focusDistance);
    separationShader->UniformData("focusRange", m_focusRange);
    separationShader->UniformData("zNear", scene.Camera()->ZNear());
    separationShader->UniformData("zFar", scene.Camera()->ZFar());

    gpu::TexturePtr depth = ResourcesManager::Instance()->GetTexture(GBuffer::GBUFFER_DEPTH_NAME);
    gpu::TexturePtr frame = ResourcesManager::Instance()->GetTexture(Bloom::BLOOM_NAME);
    gpu::TexturePtr separation = ResourcesManager::Instance()->GetTexture(DOF_SEPARATION_NAME);

    depth->Bind(0);
    separation->BindImage(1, 0, gpu::GLUtils::Write);

    separationShader->Dispatch(std::ceil(m_intermediateWidth / 16) + 1, std::ceil(m_intermediateHeight / 16) + 1, 1);
    separationShader->Wait();

    // blur the near field in the 2 axis
    gpu::ShaderPtr blurNearShader = ResourcesManager::Instance()->GetShader(DOF_NEAR_BLUR_SHADER_NAME);
    blurNearShader->Bind();
    blurNearShader->UniformData("direction", 0);

    gpu::TexturePtr tempBlur = ResourcesManager::Instance()->GetTexture(DOF_TEMP_BLUR_NAME);
    gpu::TexturePtr nearBlur = ResourcesManager::Instance()->GetTexture(DOF_NEAR_BLUR_NAME);

    frame->Bind(0);
    blurNearShader->UniformData("frame", 0);
    separation->Bind(1);
    blurNearShader->UniformData("coc", 1);
    tempBlur->BindImage(2, 0, gpu::GLUtils::Write);

    blurNearShader->Dispatch(std::ceil(m_intermediateWidth / 16) + 1, std::ceil(m_intermediateHeight / 16) + 1, 1);
    blurNearShader->Wait();

    blurNearShader->UniformData("direction", 1);
    tempBlur->Bind(0);
    nearBlur->BindImage(2, 0, gpu::GLUtils::ReadWrite);
    blurNearShader->Dispatch(std::ceil(m_intermediateWidth / 16) + 1, std::ceil(m_intermediateHeight / 16) + 1, 1);
    blurNearShader->Wait();

    // blur the far field in the 2 axis
    gpu::ShaderPtr blurFarShader = ResourcesManager::Instance()->GetShader(DOF_FAR_BLUR_SHADER_NAME);
    blurFarShader->Bind();
    blurFarShader->UniformData("direction", 0);

    gpu::TexturePtr farBlur = ResourcesManager::Instance()->GetTexture(DOF_FAR_BLUR_NAME);

    frame->Bind(0);
    blurFarShader->UniformData("frame", 0);
    separation->Bind(1);
    blurFarShader->UniformData("coc", 1);
    tempBlur->BindImage(2, 0, gpu::GLUtils::Write);

    blurFarShader->Dispatch(std::ceil(m_intermediateWidth / 16) + 1, std::ceil(m_intermediateHeight / 16) + 1, 1);
    blurFarShader->Wait();

    blurFarShader->UniformData("direction", 1);
    tempBlur->Bind(0);
    farBlur->BindImage(2, 0, gpu::GLUtils::ReadWrite);
    blurFarShader->Dispatch(std::ceil(m_intermediateWidth / 16) + 1, std::ceil(m_intermediateHeight / 16) + 1, 1);
    blurFarShader->Wait();

    // composite into the final result
    gpu::ShaderPtr compositeShader = ResourcesManager::Instance()->GetShader(DOF_COMPOSITE_SHADER_NAME);
    compositeShader->Bind();

    gpu::TexturePtr result = ResourcesManager::Instance()->GetTexture(DOF_NAME);

    separation->Bind(0);
    compositeShader->UniformData("coc", 0);
    nearBlur->Bind(1);
    compositeShader->UniformData("nearBlur", 1);
    farBlur->Bind(2);
    compositeShader->UniformData("farBlur", 2);
    frame->Bind(3);
    compositeShader->UniformData("frame", 3);
    result->BindImage(4, 0, gpu::GLUtils::Write);

    compositeShader->Dispatch(std::ceil(m_width / 16) + 1, std::ceil(m_height / 16) + 1, 1);
    compositeShader->Wait();
}

void DepthOfField::RenderUI()
{
    if (ImGui::TreeNode("Depth of Field"))
    {
        ImGui::InputFloat("Focus distance", &m_focusDistance);
        ImGui::InputFloat("Focus range", &m_focusRange);
        if (ImGui::CollapsingHeader("Preview"))
        {
            ImGui::Text("Separation");
            IMGUI_PASS_DEBUG_IMAGE_OPENGL(DOF_SEPARATION_NAME);

            ImGui::Text("Near Blur");
            IMGUI_PASS_DEBUG_IMAGE_OPENGL(DOF_NEAR_BLUR_NAME);

            ImGui::Text("Far Blur");
            IMGUI_PASS_DEBUG_IMAGE_OPENGL(DOF_FAR_BLUR_NAME);
        }
        ImGui::TreePop();
    }
}

void DepthOfField::Rebuild(uint32_t width, uint32_t height)
{
    m_width = width;
    m_height = height;
    m_intermediateWidth = width / 2;
    m_intermediateHeight = height / 2;

    gpu::TexturePtr separation = ResourcesManager::Instance()->GetTexture(DOF_SEPARATION_NAME);
    separation->SetSize(static_cast<int>(m_intermediateWidth), static_cast<int>(m_intermediateHeight));
    separation->Reallocate();

    gpu::TexturePtr tempBlur = ResourcesManager::Instance()->GetTexture(DOF_TEMP_BLUR_NAME);
    tempBlur->SetSize(static_cast<int>(m_intermediateWidth), static_cast<int>(m_intermediateHeight));
    tempBlur->Reallocate();

    gpu::TexturePtr nearBlur = ResourcesManager::Instance()->GetTexture(DOF_NEAR_BLUR_NAME);
    nearBlur->SetSize(static_cast<int>(m_intermediateWidth), static_cast<int>(m_intermediateHeight));
    nearBlur->Reallocate();

    gpu::TexturePtr farBlur = ResourcesManager::Instance()->GetTexture(DOF_FAR_BLUR_NAME);
    farBlur->SetSize(static_cast<int>(m_intermediateWidth), static_cast<int>(m_intermediateHeight));
    farBlur->Reallocate();

    gpu::TexturePtr dof = ResourcesManager::Instance()->GetTexture(DOF_NAME);
    dof->SetSize(static_cast<int>(m_width), static_cast<int>(m_height));
    dof->Reallocate();
}
} // lum::rdr