//
// Created by belle on 02/03/2026.
//

#include "Script.h"

#include <filesystem>

#include "Lumiere/EngineCfg.h"
#include "Lumiere/ScriptEngine.h"

namespace lum::comp
{
REGISTER_TO_COMPONENT_FACTORY(Script, "Script");

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

std::string Script::Name() const
{
    size_t name = m_path.find_last_of('/');

    return m_path.substr(name + 1, m_path.length() - 1);
}

void Script::Serialize(YAML::Node node)
{
    std::filesystem::path path(m_path);
    if (path.is_absolute())
        path = path.lexically_relative(cfg::EXECUTABLE_DIR);

    YAML::Node s;
    s["componentType"] = "Script";
    s["path"] = m_path.empty() ? "" : path.string();
    node.push_back(s);
}

void Script::Deserialize(YAML::Node node)
{
    SetScriptPath(node["path"].as<std::string>());
}

} // lum::comp