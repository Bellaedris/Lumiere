//
// Created by belle on 02/03/2026.
//

#include "ScriptEngine.h"

namespace lum
{
ScriptEngine::ScriptEngine()
{
    m_state.open_libraries(sol::lib::base);
    m_state.open_libraries(sol::lib::math);
    m_state.script("print('Lua Scripting engine started')");
}
}
