//
// Created by belle on 23/02/2026.
//

#include "Transform.h"

#include "sol.hpp"
#include "Lumiere/Node3D.h"
#include "Lumiere/ScriptEngine.h"
#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui_internal.h"

namespace lum::comp
{
bool Transform::registered = false;

Transform::Transform(Node3D* node)
    : IComponent(node)
{
    // if not done already, register the transform type to the Lua state
    if (registered == false)
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

        registered = true;
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

void Transform::DrawInspector()
{
    static ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen;
    if (ImGui::TreeNodeEx(ICON_FA_ARROWS " Transform", flags))
    {
        ImGui::BeginTable("Transform", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_PadOuterX);

        ImGui::TableNextRow();
        TransformSlider("Position", LocalPosition(), .0f, [&](const glm::vec3& vector)
        {
            SetLocalPosition(vector);
        });

        ImGui::TableNextRow();
        TransformSlider("Rotation", LocalRotation(), .0f, [&](const glm::vec3& vector)
        {
            SetLocalRotation(vector);
        });

        ImGui::TableNextRow();
        TransformSlider("Scale", LocalScale(), 1.f, [&](const glm::vec3& vector)
        {
            SetLocalScale(vector);
        });
        ImGui::EndTable();
        ImGui::TreePop();
    }
}

void Transform::TransformSlider(const char* name, glm::vec3 vector, float defaultValue, std::function<void(const glm::vec3&)> updateVector)
{
    ImGui::AlignTextToFramePadding();

    ImGui::TableNextColumn();
    ImGui::Text("%s", name);

    ImGui::TableNextColumn();
    ImGui::PushMultiItemsWidths(3, ImGui::GetContentRegionAvail().x - 30);

    ImGui::PushID(name);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

    //ImGui::PushStyleColor(ImGuiCol_ButtonActive, EditorCol_Secondary2);
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0, 1));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0, 1));
        {
            if (ImGui::Button("X")) {
                updateVector(glm::vec3(defaultValue, vector.y, vector.z));
            }
            ImGui::SameLine();
            if (ImGui::DragFloat("##X", &vector.x, 0.5f))
            {
                updateVector(vector);
            }
            ImGui::SameLine();
        }
        ImGui::PopStyleColor(2); // Button, ButtonHovered
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 1, 0, 1));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 1, 0, 1));
        {
            if (ImGui::Button("Y")) {
                updateVector(glm::vec3(vector.x, defaultValue, vector.z));
            }
            ImGui::SameLine();
            if (ImGui::DragFloat("##Y", &vector.y, 0.5f))
            {
                updateVector(vector);
            }
            ImGui::SameLine();
        }
        ImGui::PopStyleColor(2); // Button, ButtonHovered
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 1, 1));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 1, 1));
        {
            if (ImGui::Button("Z"))
            {
                updateVector(glm::vec3(vector.x, vector.y, defaultValue));
            }
            ImGui::SameLine();
            if (ImGui::DragFloat("##Z", &vector.z, 0.5f))
            {
                updateVector(vector);
            }
        }
        ImGui::PopStyleColor(2); // Button, ButtonHovered
    }

    ImGui::PopStyleVar();
    ImGui::PopID();
}
} // lum