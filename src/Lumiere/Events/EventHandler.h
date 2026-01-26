//
// Created by Bellaedris on 26/01/2026.
//

#pragma once
#include <functional>
#include <map>
#include <memory>
#include <vector>

#include "IEvent.h"

namespace lum::evt
{
class EventHandler
{
private:
    std::map<EventType, std::vector<std::function<void(const std::shared_ptr<IEvent>& e)>>> m_listeners;
public:
    void Subscribe(EventType type, const std::function<void(const std::shared_ptr<IEvent> &e)> &callback);
    void Emit(const std::shared_ptr<IEvent>& e);
};
} // lum::evt