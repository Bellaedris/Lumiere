//
// Created by Bellaedris on 15/04/2026.
//

#include "UI.h"

#include "ChromaticAberration.h"
#include "ShadePBR.h"
#include "Lumiere/RendererManager.h"
#include "Lumiere/ResourcesManager.h"

namespace lum::rdr
{

REGISTER_TO_PASS_FACTORY(UI, UI::UI_NAME);

UI::UI(uint32_t width, uint32_t height)
    : m_width(width)
    , m_height(height)
    , m_framebuffer(std::make_unique<gpu::Framebuffer>(width, height))
{
    UI::Init();
}

void UI::Init()
{
    gpu::TexturePtr albedo = ResourcesManager::Instance()->GetTexture(RendererManager::RENDERED_FRAME_NAME);
    m_framebuffer->Attach(gpu::Framebuffer::Color, albedo, 0);

    std::vector<gpu::Shader::ShaderSource> shaderSources =
    {
        {gpu::Shader::Vertex, "shaders/ui.vert"},
        {gpu::Shader::Fragment, "shaders/ui.frag"}
    };
    ResourcesManager::Instance()->CacheShader(UI_SHADER_NAME, shaderSources);
}

void UI::Render(const FrameData &frameData)
{
    if (frameData.profilerGPU)
        frameData.profilerGPU->BeginScope("UI");

    m_framebuffer->Bind(gpu::Framebuffer::ReadWrite);

    gpu::ShaderPtr uiShader = ResourcesManager::Instance()->GetShader(UI_SHADER_NAME);
    uiShader->Bind();

    for (const auto& instance : frameData.scene->UIRenderInstances())
    {
        instance.first->Bind(uiShader);
        for (const auto& submesh : instance.second)
        {
            uiShader->UniformData("modelMatrix", submesh.model);
            submesh.mesh->Draw();
        }
    }

    m_framebuffer->Unbind(gpu::Framebuffer::ReadWrite);

    if (frameData.profilerGPU)
        frameData.profilerGPU->EndScope("UI");
}

void UI::RenderUI()
{

}

void UI::Rebuild(uint32_t width, uint32_t height)
{
    m_width = width;
    m_height = height;
    m_framebuffer->SetSize(width, height);
}

void UI::Serialize(YAML::Node passes)
{
    YAML::Node ui;
    ui["name"] = UI_NAME;
    ui["width"] = m_width;
    ui["height"] = m_height;
    passes.push_back(ui);
}

void UI::Deserialize(YAML::Node pass)
{
    m_width = pass["width"].as<uint32_t>();
    m_height = pass["height"].as<uint32_t>();
    m_framebuffer = std::make_unique<gpu::Framebuffer>(m_width, m_height);
    Init();
}
}
