//
// Created by Bellaedris on 20/01/2026.
//

#include "RenderPipeline.h"

#include <memory>

#include "Lumiere/ResourcesManager.h"
#include "Lumiere/Events/RenderEvents.h"

namespace lum::rdr {
RenderPipeline::RenderPipeline(const std::shared_ptr<evt::EventHandler>& handler)
    : m_eventHandler(handler)
{
    m_cameraData = std::make_unique<lum::gpu::Buffer>(lum::gpu::Buffer::BufferType::Uniform);
    LUM_SUB_TO_EVENT(m_eventHandler, evt::EventType::FramebufferResized, RenderPipeline::OnEvent);
}

void RenderPipeline::Render(const FrameData &frameData) const
{
    // upload camera data
    CameraData cameraData = {frameData.scene->Camera()->View(), frameData.scene->Camera()->Projection(), frameData.scene->Camera()->Position()};
    m_cameraData->Write(sizeof(CameraData), &cameraData, lum::gpu::Buffer::DynamicDraw);
    m_cameraData->Bind(0);

    // upload light datas so the passes can use them
    frameData.scene->Lights()->Update();
    frameData.scene->Lights()->Bind(1, 2, 3);

    for (const auto& pass : m_passes)
        pass->Render(frameData);

    gpu::TexturePtr frame = ResourcesManager::Instance()->GetTexture(RENDERED_FRAME_NAME);
    m_eventHandler->Emit(std::make_shared<evt::FrameRenderedEvent>(frame));
}

void RenderPipeline::RenderUI()
{
    for (const std::shared_ptr<IPass>& pass : m_passes)
        pass->RenderUI();
}

void RenderPipeline::AddPass(const std::shared_ptr<IPass>& pass)
{
    m_passes.push_back(pass);
}

void RenderPipeline::Rebuild(const glm::vec2 &newSize)
{
    for (const auto& pass : m_passes)
        pass->Rebuild(static_cast<uint32_t>(newSize.x), static_cast<uint32_t>(newSize.y));
}

void RenderPipeline::OnEvent(const std::shared_ptr<evt::IEvent> &e)
{
    if (e->Type() == evt::FramebufferResized)
    {
        auto event = std::dynamic_pointer_cast<evt::FramebufferResizedEvent>(e);
        Rebuild(event->m_size);
    }
}
} // lum::rdr