//
// Created by belle on 02/03/2026.
//

#include "ScriptEngine.h"

#include "Lumiere/InputManager.h"
#include "Lumiere/Node3D.h"
#include "CameraSystem.h"
#include "Lumiere/Components/Script.h"
#include "Lumiere/Renderer/RenderSettings.h"

namespace lum
{
ScriptEngine::ScriptEngine(CameraSystem* cameraSystem, rdr::RenderSettings* settings)
    : m_scriptEvents(std::make_unique<evt::ScriptEventHandler>())
    , m_cameraSystem(cameraSystem)
    , m_renderSettings(settings)
{
    m_state.open_libraries(sol::lib::base);
    m_state.open_libraries(sol::lib::math);
    m_state.script("print('Lua Scripting engine started')");

    // also register types that do not have dependencies on component-related types,
    // here inputs
    sol::table keyCode = m_state.create_table();
    keyCode["Unknown"] = KeyCode::lKeyUnknown;
    keyCode["W"] = KeyCode::lKeyW;
    keyCode["A"] = KeyCode::lKeyA;
    keyCode["S"] = KeyCode::lKeyS;
    keyCode["D"] = KeyCode::lKeyD;
    keyCode["Q"] = KeyCode::lKeyQ;
    keyCode["E"] = KeyCode::lKeyE;
    keyCode["Alt"] = KeyCode::lKeyAlt;
    keyCode["Space"] = KeyCode::lKeySpace;
    keyCode["Ctrl"] = KeyCode::lKeyCtrl;
    keyCode["Shift"] = KeyCode::lKeyShift;
    m_state["KeyCode"] = keyCode;

    sol::table mouseButton = m_state.create_table();
    mouseButton["LeftClick"] = MouseButton::lLeftClick;
    mouseButton["RightClick"] = MouseButton::lRightClick;
    mouseButton["CtrMiddleClick"] = MouseButton::lMiddleClick;
    m_state["MouseButton"] = mouseButton;

    m_state["IsKeyDown"] = [](float k) {return InputManager::IsKeyDown(static_cast<KeyCode>(k));};
    m_state["IsKeyPressed"] = [](float k) {return InputManager::IsKeyPressed(static_cast<KeyCode>(k));};
    m_state["IsKeyReleased"] = [](float k) {return InputManager::IsKeyReleased(static_cast<KeyCode>(k));};
    m_state["IsMouseButtonDown"] = [](float k) {return InputManager::IsMouseButtonDown(static_cast<MouseButton>(k));};
    m_state["IsMouseButtonPressed"] = [](float k) {return InputManager::IsMouseButtonPressed(static_cast<MouseButton>(k));};
    m_state["IsMouseButtonReleased"] = [](float k) {return InputManager::IsMouseButtonReleased(static_cast<MouseButton>(k));};
    m_state["GetAxis"] = &InputManager::GetAxis;

    // register event system
    sol::table events = m_state.create_table();
    events["Subscribe"] = [&](const std::string& s, const sol::function& callback){ m_scriptEvents->Subscribe(s, callback); };
    events["Emit"] = [&](const std::string& s, const sol::variadic_args &args) { m_scriptEvents->Emit(s, args); };
    m_state["Events"] = events;

    m_state["Message"] = [this](Node3D* node, const std::string& callbackName, const sol::variadic_args& args)
    {
        std::optional<comp::Script*> s = node->GetComponent<comp::Script>();
        if (s.has_value() == false)
            return;
        sol::environment& env = GetScriptContext(s.value()->Handle());
        sol::function callback = env[callbackName];
        if (callback.valid())
        {
            sol::protected_function_result res = callback(args);
            if (res.valid() == false)
            {
                sol::error err = res;
                std::cerr << "[Runtime Error] " << "\n" << err.what() << std::endl;
            }
        }
    };

    // Camera/viewport settings
    m_state["SetCursorVisible"] = [this](const bool visible){ m_cameraSystem->SetCursorVisible(visible); };

    // Render settings
    sol::usertype<rdr::RenderSettings> s = m_state.new_usertype<rdr::RenderSettings>("RenderSettings");
    s["activePipeline"] = sol::property(
        [this]() { return m_renderSettings->m_activePipeline; },
        [this](int id) { m_renderSettings->m_activePipeline = id; m_renderSettings->m_shouldSwitchPipeline = true; }
    );

    s["cameraSensorIso"] = sol::property(
        [this]() { return m_renderSettings->m_cameraSensorIso; },
        [this](float iso) { m_renderSettings->m_cameraSensorIso = iso; }
    );
    m_state["RenderSettings"] = m_renderSettings;
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

sol::environment & ScriptEngine::GetScriptContext(const ScriptHandle &handle)
{
    return m_scripts.get(handle)->m_env;
}
}
