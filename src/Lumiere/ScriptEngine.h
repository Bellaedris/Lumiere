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

    static ScriptEngine* m_instance;
public:
    ScriptEngine() = default;
    // do not copy/move a singleton
    ScriptEngine(ScriptEngine& instance) = delete;
    ScriptEngine(ScriptEngine&& instance) = delete;
    ScriptEngine& operator=(const ScriptEngine& instance) = delete;
    ScriptEngine& operator=(const ScriptEngine&& instance) = delete;

    ~ScriptEngine() { m_instance = nullptr; }

    static sol::state& Instance();
};
}