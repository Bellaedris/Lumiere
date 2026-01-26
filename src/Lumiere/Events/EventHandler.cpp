//
// Created by Bellaedris on 26/01/2026.
//

#include "EventHandler.h"

namespace lum::evt
{
void EventHandler::Subscribe(EventType type, const std::function<void(const std::shared_ptr<IEvent> &e)> &callback)
{
    if (m_listeners.contains(type))
        m_listeners[type].push_back(callback);
    else
    {
        m_listeners[type] = std::vector<std::function<void(const std::shared_ptr<IEvent>&)>>();
        m_listeners[type].push_back(callback);
    }
}

void EventHandler::Emit(const std::shared_ptr<IEvent>& e)
{
    if (m_listeners.contains(e->Type()))
        for (const auto& callback : m_listeners[e->Type()])
        {
            callback(e);
        }
}
}
