//
// Created by Bellaedris on 26/01/2026.
//

#pragma once
#include "IEvent.h"
#include "../GPU/Texture.h"

namespace lum::evt
{

struct FramebufferResizedEvent : public IEvent
{
    glm::vec2 m_size;

    FramebufferResizedEvent(const glm::vec2& size) : m_size(size) {}
    EventType Type() const override { return EventType::FramebufferResized; };
};

struct FrameRenderedEvent : public IEvent
{
    std::shared_ptr<gpu::Texture> m_frameData;

    FrameRenderedEvent(const std::shared_ptr<gpu::Texture>& frameData) : m_frameData(frameData) {}
    EventType Type() const override { return EventType::FrameRendered; };
};
} // lum::evt