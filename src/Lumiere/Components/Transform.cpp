//
// Created by belle on 23/02/2026.
//

#include "Transform.h"

#include "sol.hpp"
#include "Lumiere/Node3D.h"
#include "Lumiere/ScriptEngine.h"
#include "glm/gtc/type_ptr.hpp"
#include "Lumiere/Utils/YAMLUtils.h"

namespace lum::comp
{
REGISTER_TO_COMPONENT_FACTORY(Transform, "Transform");
bool Transform::m_typeRegistered = false;

Transform::Transform(Node3D* node)
    : IComponent(node)
{
    // if not done already, register the transform type to the Lua state
    if (m_typeRegistered == false)
    {
        sol::state& lua = ScriptEngine::Instance();
        sol::usertype<glm::vec3> vec = lua.new_usertype<glm::vec3>("vec3",
            sol::call_constructor,
            sol::constructors<glm::vec3(float), glm::vec3(float, float, float)>()
        );
        vec["x"] = sol::property(
            [](const glm::vec3& v) { return v.x; },
            [](glm::vec3& v, float x) { v.x = x; }
        );
        vec["y"] = sol::property(
            [](const glm::vec3& v) { return v.y; },
            [](glm::vec3& v, float y) { v.y = y; }
        );
        vec["z"] = sol::property(
            [](const glm::vec3& v) { return v.z; },
            [](glm::vec3& v, float z) { v.z = z; }
        );

        sol::usertype<Transform> type = lua.new_usertype<Transform>("Transform");

        type["position"] = sol::property(
            [](Transform& t) -> glm::vec3& { return t.m_position; },
            [](Transform& t, const glm::vec3& p) { t.SetLocalPosition(p); }
        );
        type["Translate"] = &Transform::Translate;

        type["rotation"] = sol::property(
            [](Transform& t) -> glm::vec3& { return t.m_rotationEuler; },
            [](Transform& t, const glm::vec3& p) { t.m_rotationEuler = p; }
        );

        type["scale"] = sol::property(
            [](Transform& t) -> glm::vec3& { return t.m_scale; },
            [](Transform& t, const glm::vec3& p) { t.m_scale = p; }
        );

        type["worldPosition"] = &Transform::Position;
        type["worldScale"] = &Transform::Scale;

        m_typeRegistered = true;
    }
}

glm::mat4 Transform::LocalModelMatrix() const
{
    const glm::mat4 identity(1.f);
    glm::mat4       rot(1.f);
    glm::mat4       rotX = glm::rotate(identity, glm::radians(m_rotationEuler.x), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4       rotY = glm::rotate(identity, glm::radians(m_rotationEuler.y), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4       rotZ = glm::rotate(identity, glm::radians(m_rotationEuler.z), glm::vec3(0.0f, 0.0f, 1.0f));

    rot = rotY * rotX * rotZ;

    return glm::translate(identity, m_position) * rot * glm::scale(identity, m_scale);
}

void Transform::UpdateModelMatrix()
{
    m_model = LocalModelMatrix();
    m_isDirty = false;
}

void Transform::UpdateModelMatrix(const glm::mat4 &parent)
{
    m_model = parent * LocalModelMatrix();
    m_isDirty = false;
}

void Transform::SetLocalPosition(const glm::vec3 &newPosition)
{
    m_position = newPosition;
    m_isDirty = true;
}

void Transform::SetLocalRotation(const glm::vec3 &newRotation)
{
    m_rotationEuler = newRotation;
    m_isDirty = true;
}

void Transform::SetLocalScale(const glm::vec3 &newScale)
{
    m_scale = newScale;
    m_isDirty = true;
}

void Transform::Translate(const glm::vec3 &t)
{
    m_position += t;
    m_isDirty = true;
}

void Transform::Serialize(YAML::Node node)
{
    YAML::Node t;
    t["componentType"] = "Transform";
    t["position"] = m_position;
    t["rotation"] = m_rotationEuler;
    t["scale"] = m_scale;
    node["transform"] = t;
}

void Transform::Deserialize(YAML::Node node)
{
    m_position = node["position"].as<glm::vec3>();
    m_rotationEuler = node["rotation"].as<glm::vec3>();
    m_scale = node["scale"].as<glm::vec3>();
    m_isDirty = true;
}
} // lum