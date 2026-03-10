//
// Created by belle on 06/03/2026.
//

#include "Light.h"

#include <glm/gtc/type_ptr.hpp>

#include "Lumiere/ScriptEngine.h"
#include "Lumiere/Utils/YAMLUtils.h"

namespace lum::comp
{
bool Light::m_typeRegistered = false;
REGISTER_TO_COMPONENT_FACTORY(Light, "Light");

void Light::RegisterType()
{
    sol::state& lua = ScriptEngine::Instance();

    sol::usertype<Light> type = lua.new_usertype<Light>("Light");

    type["color"] = sol::property(
        [](Light& l) { return l.m_color; },
        [](Light& l, const glm::vec3& c) { return l.m_color = c; }
    );

    type["intensity"] = sol::property(
        [](Light& l) { return l.m_intensity; },
        [](Light& l, float i) { return l.m_intensity = i; }
    );

    type["range"] = sol::property(
        [](Light& l) { return l.m_pointRange; },
        [](Light& l, float r) { return l.m_pointRange = r; }
    );

    m_typeRegistered = true;
}

Light::Light(Node3D *node)
    : IComponent(node)
{
    if (m_typeRegistered == false)
        RegisterType();
}

void Light::Serialize(YAML::Node node)
{
    YAML::Node light;
    light["componentType"] = "Light";
    light["type"] = m_selectedType;
    light["color"] = m_color;
    light["intensity"] = m_intensity;
    light["range"] = m_pointRange;
    node.push_back(light);
}

void Light::Deserialize(YAML::Node node)
{
    m_selectedType = node["type"].as<int>();
    m_color = node["color"].as<glm::vec3>();
    m_intensity = node["intensity"].as<float>();
    m_pointRange = node["range"].as<float>();
}
} // lum::comp