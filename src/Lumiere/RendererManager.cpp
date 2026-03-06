//
// Created by belle on 23/02/2026.
//

#include "RendererManager.h"

#include "ResourcesManager.h"
#include "Events/RenderEvents.h"

namespace lum
{
RendererManager::RendererManager(int width, int height, const std::shared_ptr<evt::EventHandler> &eventHandler)
    : m_eventHandler(eventHandler)
    , m_cameraData(std::make_unique<lum::gpu::Buffer>(lum::gpu::Buffer::BufferType::Uniform))
    , m_gBuffer(std::make_unique<rdr::GBuffer>(width, height))
{
    LUM_SUB_TO_EVENT(m_eventHandler, evt::EventType::FramebufferResized, RendererManager::OnEvent);

    // Create the target final texture
    gpu::Texture::TextureDesc output
    {
        .target = gpu::Texture::Target2D,
        .width = width,
        .height = height,
        .format = gpu::Texture::RGBA,
        .dataType = gpu::GLUtils::UnsignedByte,
        .minFilter = gpu::Texture::Nearest,
        .magFilter = gpu::Texture::Nearest,
        .wrapMode = gpu::Texture::WrapMode::Repeat
    };

    ResourcesManager::Instance()->CreateTexture(RENDERED_FRAME_NAME, output);
}

void RendererManager::AddPipeline(rdr::RenderPipeline &pipeline)
{
    m_pipelines.push_back(std::move(pipeline));
}

void RendererManager::SetActivePipeline(int pipelineId)
{
    if (pipelineId < 0 || static_cast<size_t>(pipelineId) >= m_pipelines.size())
    {
        std::cerr << "Couldn't activate out of bounds pipeline\n";
        return;
    }

    m_activePipeline = pipelineId;
}

void RendererManager::Render(const rdr::FrameData &frameData)
{
    // upload camera data
    CameraData cameraData = {frameData.scene->Camera()->View(), frameData.scene->Camera()->Projection(), frameData.scene->Camera()->Position()};
    m_cameraData->Write(sizeof(CameraData), &cameraData, lum::gpu::Buffer::DynamicDraw);
    m_cameraData->Bind(0);

    // upload light datas so the passes can use them
    frameData.scene->GatherAndUploadLights();
    frameData.scene->Lights()->Update();
    frameData.scene->Lights()->Bind(1, 2, 3);

    m_gBuffer->Render(frameData);

    m_pipelines[m_activePipeline].Render(frameData);
    gpu::TexturePtr frame = ResourcesManager::Instance()->GetTexture(RENDERED_FRAME_NAME);
    m_eventHandler->Emit(std::make_shared<evt::FrameRenderedEvent>(frame));
}

void RendererManager::RenderUI()
{
    m_pipelines[m_activePipeline].RenderUI();
}

void RendererManager::OnEvent(const std::shared_ptr<evt::IEvent> &e)
{
    if (e->Type() == evt::FramebufferResized)
    {
        auto event = std::dynamic_pointer_cast<evt::FramebufferResizedEvent>(e);
        gpu::TexturePtr output = ResourcesManager::Instance()->GetTexture(RENDERED_FRAME_NAME);
        output->SetSize(static_cast<int>(event->m_size.x), static_cast<int>(event->m_size.y));
        output->Reallocate();
        m_gBuffer->Rebuild(static_cast<int>(event->m_size.x), static_cast<int>(event->m_size.y));
        for (auto& pipeline : m_pipelines)
            pipeline.Rebuild(event->m_size);
    }
}

void RendererManager::Serialize()
{
    for (const auto& pipeline : m_pipelines)
        pipeline.Serialize();
}
} // lum