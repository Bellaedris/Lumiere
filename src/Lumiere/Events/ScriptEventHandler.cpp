//
// Created by belle on 31/03/2026.
//

#include "ScriptEventHandler.h"

namespace lum::evt
{
void ScriptEventHandler::Subscribe(const std::string &name, const sol::function &callback)
{
    if (m_listeners.contains(name) == false)
        m_listeners[name] = std::vector<sol::function>();
    m_listeners[name].push_back(callback);
}

void ScriptEventHandler::Emit(const std::string &name)
{
    if (m_listeners.contains(name))
        for (auto& function : m_listeners[name])
        {
            sol::protected_function_result res = function();
            if (res.valid() == false)
            {
                sol::error err = res;
                std::cerr << "[Runtime Error] " << "\n" << err.what() << std::endl;
            }
        }
}
} // lum