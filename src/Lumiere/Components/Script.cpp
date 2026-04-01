//
// Created by belle on 02/03/2026.
//

#include "Script.h"

#include <filesystem>

#include "Lumiere/EngineCfg.h"
#include "../Systems/ScriptEngine.h"

namespace lum::comp
{
REGISTER_TO_COMPONENT_FACTORY(Script, "Script");

bool Script::m_typeRegistered {false};

Script::Script(Node3D *node, SystemProvider* systems)
    : IComponent(node, systems)
    , m_engine(systems->m_scripting)
{
    if (m_typeRegistered == false)
    {
        m_typeRegistered = true;
        sol::state& state = systems->m_scripting->State();

    }
}

void Script::OnPlay()
{
    m_handle = m_engine->Register(m_path, m_node);
    m_initialized = true;
}

void Script::OnStop()
{
    m_engine->Unregister(m_handle);
    m_initialized = false;
}

void Script::LoadScript()
{
    // register the script
    if (m_initialized)
    {
        m_engine->Recreate(m_handle, m_path);
    }
}

void Script::SetScriptPath(const std::string &path)
{
    m_path = path;
    LoadScript();
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