//
// Created by belle on 01/04/2026.
//

#pragma once
#include "IEvent.h"

namespace lum::evt
{
struct CursorStateChangeQueriedEvent : public IEvent
{
    bool m_show;

    CursorStateChangeQueriedEvent(const bool show) : m_show(show) {}
    EventType Type() const override { return EventType::CursorStateChangeQueried; };
};
}
