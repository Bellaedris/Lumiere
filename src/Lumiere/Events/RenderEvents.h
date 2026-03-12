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

/**
 * \brief Emitted when the window class receives a resize callback. Separated from the Framebuffer resize because the two
 * have different meanings: the size of the window differs from the size of the framebuffer we draw to.
 */
struct WindowResizedEvent : public IEvent
{
    glm::vec2 m_size;

    WindowResizedEvent() {}
    EventType Type() const override { return EventType::WindowResized; };
};

struct FrameRenderedEvent : public IEvent
{
    std::shared_ptr<gpu::Texture> m_frameData;

    FrameRenderedEvent(const std::shared_ptr<gpu::Texture>& frameData) : m_frameData(frameData) {}
    EventType Type() const override { return EventType::FrameRendered; };
};
} // lum::evt