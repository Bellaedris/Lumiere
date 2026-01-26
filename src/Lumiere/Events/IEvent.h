//
// Created by Bellaedris on 26/01/2026.
//

#pragma once

namespace lum::evt
{
enum EventType
{
    FrameRendered,
};

class IEvent
{
public:
    virtual ~IEvent() = default;
    virtual EventType Type() const = 0;
};
} // lum::evt