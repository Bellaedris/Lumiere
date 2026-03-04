//
// Created by belle on 02/03/2026.
//

#include "Script.h"

#include "Lumiere/ScriptEngine.h"

namespace lum::comp
{
Script::Script(Node3D *node)
    : IComponent(node)
{
    sol::state& lua = ScriptEngine::Instance();
    m_env = sol::environment(lua, sol::create, lua.globals());
    m_env["print"] = lua["print"];
    m_node->SetScriptingContext(m_env);
}

void Script::LoadScript()
{
    sol::protected_function_result res = ScriptEngine::Instance().safe_script_file(m_path, m_env, sol::script_pass_on_error);
    if (res.valid())
    {
        m_update = m_env["Update"];
        m_start = m_env["Start"];
    }
    else
    {
        sol::error err = res;
        std::cerr << "[Script Error] " << m_path << "\n" << err.what() << std::endl;
    }
    m_started = false;
}

void Script::Update(float dt)
{
    if (m_path.empty() == false)
    {
        // run start once, before first update
        if (m_started == false)
        {
            if (m_update.valid())
            {
                sol::protected_function_result res = m_start();
                if (res.valid() == false)
                {
                    sol::error err = res;
                    std::cerr << "[Runtime Error] " << m_path << "\n" << err.what() << std::endl;
                }
            }
            m_started = true;
        }

        if (m_update.valid())
        {
            sol::protected_function_result res = m_update(dt);
            if (res.valid() == false)
            {
                sol::error err = res;
                std::cerr << "[Runtime Error] " << m_path << "\n" << err.what() << std::endl;
            }
        }
    }
}
} // lum::comp