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
    ResourcesManager::Instance()->CreateTexture(DOF_NEAR_FILL_NAME, blurDesc);
    ResourcesManager::Instance()->CreateTexture(DOF_FAR_FILL_NAME, blurDesc);
    ResourcesManager::Instance()->CreateTexture(DOF_NAME, finalDesc);

    std::vector<gpu::Shader::ShaderSource> sources =
    {
        {gpu::Shader::Compute, "shaders/dof_separate.comp"}
    };
    ResourcesManager::Instance()->CacheShader(DOF_PLANE_SEPARATION_SHADER_NAME, sources);

    std::vector<gpu::Shader::ShaderSource> nearExpandSources =
    {
        {gpu::Shader::Compute, "shaders/dof_expand_near.comp"}
    };
    ResourcesManager::Instance()->CacheShader(DOF_NEAR_EXPAND_SHADER_NAME, nearExpandSources);

    std::vector<gpu::Shader::ShaderSource> expandBlurSources =
    {
        {gpu::Shader::Compute, "shaders/dof_box_blur.comp"}
    };
    ResourcesManager::Instance()->CacheShader(DOF_NEAR_EXPAND_BLUR_SHADER_NAME, expandBlurSources);

    std::vector<gpu::Shader::ShaderSource> bokehBlurSources =
    {
        {gpu::Shader::Compute, "shaders/dof_bokeh_blur.comp"}
    };
    ResourcesManager::Instance()->CacheShader(DOF_BOKEH_BLUR_SHADER_NAME, bokehBlurSources);

    std::vector<gpu::Shader::ShaderSource> bokehFillSources =
    {
        {gpu::Shader::Compute, "shaders/dof_bokeh_fill.comp"}
    };
    ResourcesManager::Instance()->CacheShader(DOF_BOKEH_FILL_SHADER_NAME, bokehFillSources);

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
    gpu::TexturePtr tempBlur = ResourcesManager::Instance()->GetTexture(DOF_TEMP_BLUR_NAME);

    depth->Bind(0);
    separation->BindImage(1, 0, gpu::GLUtils::Write);

    separationShader->Dispatch(std::ceil(m_intermediateWidth / 16) + 1, std::ceil(m_intermediateHeight / 16) + 1, 1);
    separationShader->Wait();

    // expand the near field to have a smooth transition between near and far
    if (m_expansionSize > 0 && m_expansionBlur > 0)
    {
        gpu::ShaderPtr expandShader = ResourcesManager::Instance()->GetShader(DOF_NEAR_EXPAND_SHADER_NAME);
        expandShader->Bind();

        separation->Bind(0);
        tempBlur->BindImage(1, 0, gpu::GLUtils::Write);

        expandShader->Dispatch(std::ceil(m_intermediateWidth / 16) + 1, std::ceil(m_intermediateHeight / 16) + 1, 1);
        expandShader->Wait();

        // blur the near field for a smooth transition between near and in focus
        gpu::ShaderPtr expandBlurShader = ResourcesManager::Instance()->GetShader(DOF_NEAR_EXPAND_BLUR_SHADER_NAME);
        expandBlurShader->Bind();

        tempBlur->Bind(0);
        separation->BindImage(1, 0, gpu::GLUtils::Write);

        expandBlurShader->Dispatch(std::ceil(m_intermediateWidth / 16) + 1, std::ceil(m_intermediateHeight / 16) + 1, 1);
        expandBlurShader->Wait();
    }

    // apply a bokeh blur
    gpu::ShaderPtr bokehShader = ResourcesManager::Instance()->GetShader(DOF_BOKEH_BLUR_SHADER_NAME);
    bokehShader->Bind();

    gpu::TexturePtr nearBlur = ResourcesManager::Instance()->GetTexture(DOF_NEAR_BLUR_NAME);
    gpu::TexturePtr farBlur = ResourcesManager::Instance()->GetTexture(DOF_FAR_BLUR_NAME);

    frame->Bind(0);
    bokehShader->UniformData("frame", 0);
    separation->Bind(1);
    bokehShader->UniformData("coc", 1);
    nearBlur->BindImage(2, 0, gpu::GLUtils::Write);
    farBlur->BindImage(3, 0, gpu::GLUtils::Write);

    bokehShader->Dispatch(std::ceil(m_intermediateWidth / 16) + 1, std::ceil(m_intermediateHeight / 16) + 1, 1);
    bokehShader->Wait();

    // our bokeh blur pattern leaves holes inbetween its texels, so we must fill it
    gpu::ShaderPtr bokehFillShader = ResourcesManager::Instance()->GetShader(DOF_BOKEH_FILL_SHADER_NAME);
    bokehFillShader->Bind();

    gpu::TexturePtr nearFill = ResourcesManager::Instance()->GetTexture(DOF_NEAR_FILL_NAME);
    gpu::TexturePtr farFill = ResourcesManager::Instance()->GetTexture(DOF_FAR_FILL_NAME);

    separation->Bind(0);
    nearBlur->BindImage(1, 0, gpu::GLUtils::Read);
    farBlur->BindImage(2, 0, gpu::GLUtils::Read);
    nearFill->BindImage(3, 0, gpu::GLUtils::Write);
    farFill->BindImage(4, 0, gpu::GLUtils::Write);

    bokehFillShader->Dispatch(std::ceil(m_intermediateWidth / 16) + 1, std::ceil(m_intermediateHeight / 16) + 1, 1);
    bokehFillShader->Wait();

    // blurNearShader->UniformData("direction", 1);
    // tempBlur->Bind(0);
    // nearBlur->BindImage(2, 0, gpu::GLUtils::ReadWrite);
    // blurNearShader->Dispatch(std::ceil(m_intermediateWidth / 16) + 1, std::ceil(m_intermediateHeight / 16) + 1, 1);
    // blurNearShader->Wait();

    // blur the far field in the 2 axis
    // gpu::ShaderPtr blurFarShader = ResourcesManager::Instance()->GetShader(DOF_FAR_BLUR_SHADER_NAME);
    // blurFarShader->Bind();
    // blurFarShader->UniformData("direction", 0);
    //
    // frame->Bind(0);
    // blurFarShader->UniformData("frame", 0);
    // separation->Bind(1);
    // blurFarShader->UniformData("coc", 1);
    // tempBlur->BindImage(2, 0, gpu::GLUtils::Write);
    //
    // blurFarShader->Dispatch(std::ceil(m_intermediateWidth / 16) + 1, std::ceil(m_intermediateHeight / 16) + 1, 1);
    // blurFarShader->Wait();
    //
    // blurFarShader->UniformData("direction", 1);
    // tempBlur->Bind(0);
    // farBlur->BindImage(2, 0, gpu::GLUtils::ReadWrite);
    // blurFarShader->Dispatch(std::ceil(m_intermediateWidth / 16) + 1, std::ceil(m_intermediateHeight / 16) + 1, 1);
    // blurFarShader->Wait();

    // composite into the final result
    gpu::ShaderPtr compositeShader = ResourcesManager::Instance()->GetShader(DOF_COMPOSITE_SHADER_NAME);
    compositeShader->Bind();

    gpu::TexturePtr result = ResourcesManager::Instance()->GetTexture(DOF_NAME);

    separation->Bind(0);
    compositeShader->UniformData("coc", 0);
    nearFill->Bind(1);
    compositeShader->UniformData("nearBlur", 1);
    farFill->Bind(2);
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
        ImGui::SliderInt("Expansion Size", &m_expansionSize, 0, 10);
        ImGui::SliderInt("Expansion Blur", &m_expansionBlur, 0, 10);
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

    gpu::TexturePtr nearFill = ResourcesManager::Instance()->GetTexture(DOF_NEAR_FILL_NAME);
    nearFill->SetSize(static_cast<int>(m_intermediateWidth), static_cast<int>(m_intermediateHeight));
    nearFill->Reallocate();

    gpu::TexturePtr farFill = ResourcesManager::Instance()->GetTexture(DOF_FAR_FILL_NAME);
    farFill->SetSize(static_cast<int>(m_intermediateWidth), static_cast<int>(m_intermediateHeight));
    farFill->Reallocate();

    gpu::TexturePtr dof = ResourcesManager::Instance()->GetTexture(DOF_NAME);
    dof->SetSize(static_cast<int>(m_width), static_cast<int>(m_height));
    dof->Reallocate();
}
} // lum::rdr