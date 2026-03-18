//
// Created by belle on 02/03/2026.
//

#pragma once
#include <string>

#include "IComponent.h"
#include "sol.hpp"
#include "Lumiere/Node3D.h"
#include "Lumiere/Systems/ScriptEngine.h"

namespace lum::comp
{
class Script : public IComponent
{
private:
    static bool m_registered;

    ScriptEngine* m_engine {nullptr};

    /** \brief Path to the script that will be executed */
    std::string m_path;
    /** \brief index of the script in the scriptEngine array */
    ScriptHandle m_handle;
    bool m_initialized {false};

public:
    Script(Node3D* node, SystemProvider* systems);

    void OnPlay() override;
    void OnStop() override;

    void LoadScript();

    void        SetScriptPath(const std::string& path);;
    std::string Path() const { return m_path; }

    /**
     * \brief A readable, path-free identifier of the attached script
     * \return The name of the script, i.e. the name of the file, without the whole path but with the extension
     */
    std::string Name() const;

    void        Serialize(YAML::Node node) override;
    void        Deserialize(YAML::Node node) override;
};
} // lum::comp