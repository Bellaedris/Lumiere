//
// Created by belle on 23/02/2026.
//

#include "Transform.h"

#include "sol.hpp"
#include "Lumiere/Node3D.h"
#include "glm/gtc/type_ptr.hpp"
#include "Lumiere/Utils/YAMLUtils.h"

namespace lum::comp
{
REGISTER_TO_COMPONENT_FACTORY(Transform, "Transform");

Transform::Transform(Node3D* node, SystemProvider* systems)
    : IComponent(node, systems)
{

}

glm::mat4 Transform::LocalModelMatrix() const
{
    const glm::mat4 identity(1.f);
    glm::mat4 rotation = glm::mat4(m_rotation);
    return glm::translate(identity, m_position) * rotation * glm::scale(identity, m_scale);
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

void Transform::SetPosition(const glm::vec3 &newPosition)
{
    if (m_node->Parent() != nullptr)
    {
        glm::mat4 inverseModel = glm::inverse(m_node->Parent()->GetTransform()->Model());
        m_position = glm::vec3(inverseModel * glm::vec4(newPosition, 1));
    }
    else
    {
        m_position = newPosition;
    }
    m_isDirty = true;
}

void Transform::SetLocalPosition(const glm::vec3 &newPosition)
{
    m_position = newPosition;
    m_isDirty = true;
}

void Transform::SetRotation(const glm::quat &newRotation)
{
    if (m_node->Parent() != nullptr)
    {
        glm::quat inverseModel = glm::inverse(m_node->Parent()->GetTransform()->Rotation());
        m_rotation = inverseModel * glm::normalize(newRotation);
    }
    else
    {
        m_rotation = glm::normalize(newRotation);
    }
    m_isDirty = true;
}

void Transform::SetLocalRotation(const glm::quat &newRotation)
{
    m_rotation = newRotation;
    m_isDirty = true;
}

void Transform::SetScale(const glm::vec3 &newScale)
{
    if (m_node->Parent() != nullptr)
    {
        m_scale = m_node->Parent()->GetTransform()->Scale() * newScale;
    }
    else
    {
        m_scale = newScale;
    }
    m_isDirty = true;
}

void Transform::SetLocalScale(const glm::vec3 &newScale)
{
    m_scale = newScale;
    m_isDirty = true;
}

void Transform::SetEulerAngles(const glm::vec3 &eulerAngles)
{
    SetRotation(glm::quat(glm::radians(eulerAngles)));
}

void Transform::SetLocalEulerAngles(const glm::vec3 &eulerAngles)
{
    m_rotation = glm::quat(glm::radians(eulerAngles));
    m_isDirty = true;
}

void Transform::Translate(const glm::vec3 &t)
{
    m_position += t;
    m_isDirty = true;
}

void Transform::Rotate(const glm::vec3 &euler)
{
    glm::quat angle = glm::quat(glm::radians(euler));
    m_rotation = angle * m_rotation;
}

glm::quat Transform::Rotation() const
{
    if (m_node->Parent() != nullptr)
    {
        return m_node->Parent()->GetTransform()->Rotation() * m_rotation;
    }
    return m_rotation;
}

void Transform::Serialize(YAML::Node node)
{
    YAML::Node t;
    t["componentType"] = "Transform";
    t["position"] = m_position;
    t["rotation"] = m_rotation;
    t["scale"] = m_scale;
    node["transform"] = t;
}

void Transform::Deserialize(YAML::Node node)
{
    m_position = node["position"].as<glm::vec3>();
    m_rotation = node["rotation"].as<glm::quat>();
    m_scale = node["scale"].as<glm::vec3>();
    UpdateModelMatrix();
    m_isDirty = true;
}
} // lum