//
// Created by belle on 23/02/2026.
//

#include "Transform.h"

#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui.h"

namespace lum::comp
{
glm::mat4 Transform::LocalModelMatrix() const
{
    const glm::mat4 identity(1.f);
    glm::mat4       rot(1.f);
    glm::mat4       rotX = glm::rotate(identity, m_rotationEuler.x, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4       rotY = glm::rotate(identity, m_rotationEuler.y, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4       rotZ = glm::rotate(identity, m_rotationEuler.z, glm::vec3(0.0f, 0.0f, 1.0f));

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

void Transform::DrawInspector()
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanFullWidth;
    if (ImGui::TreeNodeEx("Transform", flags))
    {
        bool shouldUpdate = false;
        shouldUpdate |= ImGui::InputFloat3("Position", glm::value_ptr(m_position));
        shouldUpdate |= ImGui::InputFloat3("Rotation", glm::value_ptr(m_rotationEuler));
        shouldUpdate |= ImGui::InputFloat3("Scale", glm::value_ptr(m_scale));

        if (shouldUpdate)
            UpdateModelMatrix();

        ImGui::TreePop();
    }
}
} // lum