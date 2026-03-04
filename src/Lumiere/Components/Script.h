//
// Created by belle on 02/03/2026.
//

#pragma once
#include <string>

#include "IComponent.h"
#include "sol.hpp"
#include "Lumiere/Node3D.h"

namespace lum::comp
{
class Script : public IComponent
{
private:
    /** Path to the script that will be executed */
    std::string m_path;
    sol::environment m_env;

    sol::function m_start;
    sol::function m_update;

    bool m_started;

public:
    Script(Node3D* node);

    void LoadScript();

    void Update(float dt) override;

    void SetScriptPath(const std::string& path) { m_path = path; };
    std::string Path() const { return m_path; }
};
} // lum::comp