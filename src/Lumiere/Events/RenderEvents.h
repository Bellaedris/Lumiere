//
// Created by Bellaedris on 26/01/2026.
//

#pragma once
#include "IEvent.h"

namespace lum::evt
{
struct FrameRenderedEvent : public IEvent
{
    std::shared_ptr<gpu::Texture> m_frameData;

    FrameRenderedEvent(const std::shared_ptr<gpu::Texture>& frameData) : m_frameData(frameData) {}
    EventType Type() const override { return EventType::FrameRendered; };
};
} // lum::evt