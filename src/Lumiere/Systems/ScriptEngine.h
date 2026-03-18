//
// Created by belle on 02/03/2026.
//

#pragma once

#include <sol/sol.hpp>

namespace lum
{
class ScriptEngine
{
private:
    sol::state m_state;
public:
    ScriptEngine();

    sol::state& State() { return m_state; };
};
}