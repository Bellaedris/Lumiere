//
// Created by belle on 02/03/2026.
//

#pragma once

#include <limits>
#include <slot_map/slot_map.h>
#include <sol/sol.hpp>

#include "System.h"

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

    dod::slot_map<ScriptInternal> m_scripts;
public:
    ScriptEngine();

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

    sol::state& State() { return m_state; };
};
}
