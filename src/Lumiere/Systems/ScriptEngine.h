//
// Created by belle on 02/03/2026.
//

#pragma once

#include <limits>
#include <slot_map/slot_map.h>
#include <sol/sol.hpp>

#include "System.h"
#include "Lumiere/Events/ScriptEventHandler.h"
#include "Lumiere/Renderer/RenderSettings.h"

namespace lum
{
class Node3D;

struct ScriptInternal
{
    sol::environment m_env {};
    sol::function m_start {};
    sol::function m_update {};

    bool m_started {false};
};
using ScriptHandle = dod::slot_map<ScriptInternal>::key;

class ScriptEngine : public ISystem
{
private:
    sol::state m_state;
    std::unique_ptr<evt::ScriptEventHandler> m_scriptEvents;
    // We NEED the scripting to be able to access other systems, for instance the Camera system to allow for cursor manipulation!
    // It should be located here because we need a class that always exists at runtime, so it cannot be a Node that will
    // be recreated multiple times during play sessions... This constrains the ScriptEngine to be the last created object.
    // It might not be ideal, if someone has a better option.
    CameraSystem* m_cameraSystem;
    rdr::RenderSettings* m_renderSettings;

    dod::slot_map<ScriptInternal> m_scripts;
public:
    ScriptEngine(CameraSystem* cameraSystem, rdr::RenderSettings* renderSettings);

    // lifecycle functions
    void Update(float dt) override;

    ScriptHandle Register(const std::string& path, Node3D* context);
    void         Unregister(ScriptHandle handle);
    /**
     * \brief Changes a script already registered. If the script didn't exist, does nothing
     * \param handle handle to the script
     * \param path path of the script to load
     */
    void         Recreate(ScriptHandle handle, const std::string& path);

    sol::environment& GetScriptContext(const ScriptHandle& handle);

    sol::state& State() { return m_state; };
};
}
