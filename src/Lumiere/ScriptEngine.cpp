//
// Created by belle on 02/03/2026.
//

#include "ScriptEngine.h"

namespace lum
{
    ScriptEngine* ScriptEngine::m_instance = nullptr;

    sol::state& ScriptEngine::Instance()
    {
        if (m_instance == nullptr)
        {
            m_instance = new ScriptEngine();
            m_instance->m_state.open_libraries(sol::lib::base);
            m_instance->m_state.open_libraries(sol::lib::math);
            m_instance->m_state.script("print('Lua Scripting engine state created')");
        }

        return m_instance->m_state;
    }
}
