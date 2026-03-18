//
// Created by belle on 02/03/2026.
//

#include "ScriptEngine.h"

#include "Lumiere/Node3D.h"

namespace lum
{
ScriptEngine::ScriptEngine()
{
    m_state.open_libraries(sol::lib::base);
    m_state.open_libraries(sol::lib::math);
    m_state.script("print('Lua Scripting engine started')");
}

void ScriptEngine::Update(float dt)
{
    for (auto& script : m_scripts)
    {
        // run start once, before first update
        if (script.m_started == false)
        {
            if (script.m_update.valid())
            {
                sol::protected_function_result res = script.m_start();
                if (res.valid() == false)
                {
                    sol::error err = res;
                    std::cerr << "[Runtime Error] " << "\n" << err.what() << std::endl;
                }
            }
            script.m_started = true;
        }

        if (script.m_update.valid())
        {
            sol::protected_function_result res = script.m_update(dt);
            if (res.valid() == false)
            {
                sol::error err = res;
                std::cerr << "[Runtime Error] " << "\n" << err.what() << std::endl;
            }
        }
    }
}

ScriptHandle ScriptEngine::Register(const std::string &path, Node3D* context)
{
    ScriptInternal script;
    script.m_env = sol::environment(m_state, sol::create, m_state.globals());
    script.m_env["print"] = m_state["print"];

    script.m_env["node"] = context;

    sol::protected_function_result res = m_state.safe_script_file(path, script.m_env, sol::script_pass_on_error);
    if (res.valid())
    {
        script.m_update = script.m_env["Update"];
        script.m_start = script.m_env["Start"];
    }
    else
    {
        sol::error err = res;
        std::cerr << "[Script Error] " << path << "\n" << err.what() << std::endl;
    }

    return m_scripts.emplace(std::move(script));
}

void ScriptEngine::Unregister(ScriptHandle handle)
{
    m_scripts.erase(handle);
}

void ScriptEngine::Recreate(ScriptHandle handle, const std::string &path)
{
    auto script = m_scripts.get(handle);
    if (script != nullptr)
    {
        sol::protected_function_result res = m_state.safe_script_file(path, script->m_env, sol::script_pass_on_error);
        if (res.valid())
        {
            script->m_update = script->m_env["Update"];
            script->m_start = script->m_env["Start"];
        }
        else
        {
            sol::error err = res;
            std::cerr << "[Script Error] " << path << "\n" << err.what() << std::endl;
        }
    }
}
}
