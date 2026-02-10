//
// Created by Bellaedris on 10/02/2026.
//

#include "DepthOfField.h"

#include "GBuffer.h"
#include "Lumiere/ResourcesManager.h"
#include "Lumiere/Renderer/RenderPipeline.h"

namespace lum::rdr
{
DepthOfField::DepthOfField(uint32_t width, uint32_t height)
    : m_width(width)
    , m_height(height)
{
    gpu::Texture::TextureDesc nearFarDesc =
    {
        .target = gpu::Texture::Target2D,
        .width = static_cast<int>(width),
        .height = static_cast<int>(height),
        .format = gpu::Texture::RG,
        .dataType = gpu::GLUtils::Float,
        .minFilter = gpu::Texture::Nearest,
        .magFilter = gpu::Texture::Nearest,
        .wrapMode = gpu::Texture::WrapMode::ClampToEdge
    };

    ResourcesManager::Instance()->CreateTexture(DOF_SEPARATION_NAME, nearFarDesc);

    std::vector<gpu::Shader::ShaderSource> sources =
    {
        {gpu::Shader::Compute, "shaders/dof_separate.comp"}
    };
    ResourcesManager::Instance()->CacheShader(DOF_PLANE_SEPARATION_SHADER_NAME, sources);
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
    gpu::TexturePtr separation = ResourcesManager::Instance()->GetTexture(DOF_SEPARATION_NAME);

    depth->Bind(0);
    separation->BindImage(1, 0, gpu::GLUtils::Write);

    separationShader->Dispatch(std::ceil(m_width / 16) + 1, std::ceil(m_height / 16) + 1, 1);
    separationShader->Wait();
}

void DepthOfField::RenderUI()
{
    if (ImGui::TreeNode("Depth of Field"))
    {
        ImGui::InputFloat("Focus distance", &m_focusDistance);
        ImGui::InputFloat("Focus range", &m_focusRange);
        if (ImGui::CollapsingHeader("Preview"))
        {
            IMGUI_PASS_DEBUG_IMAGE_OPENGL(DOF_SEPARATION_NAME);
        }
        ImGui::TreePop();
    }
}

void DepthOfField::Rebuild(uint32_t width, uint32_t height)
{
    m_width = width;
    m_height = height;

    gpu::TexturePtr separation = ResourcesManager::Instance()->GetTexture(DOF_SEPARATION_NAME);
    separation->SetSize(static_cast<int>(width), static_cast<int>(height));
    separation->Reallocate();
}
} // lum::rdr