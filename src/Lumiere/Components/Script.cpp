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

    m_fileBrowser.SetTitle("Script selection");
    m_fileBrowser.SetTypeFilters({".lua"});
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

void Script::DrawInspector()
{
    static ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen;
    if (ImGui::TreeNodeEx(ICON_FA_FILE_CODE_O " Lua Script", flags))
    {
        if (Path().empty() == false)
        {
            ImGui::Text("Script : %s", Name().c_str());
            if (ImGui::Button("Reload Script"))
                LoadScript();
        }
        if (ImGui::Button("Select a script"))
        {
            // load a mesh from file/resourcesManager
            m_fileBrowser.Open();
        }
        ImGui::TreePop();
    }

    m_fileBrowser.Display();
    if (m_fileBrowser.HasSelected())
    {
        SetScriptPath(m_fileBrowser.GetSelected().string());
        LoadScript();
        m_fileBrowser.ClearSelected();
    }
}

} // lum::comp