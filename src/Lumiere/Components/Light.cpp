//
// Created by belle on 06/03/2026.
//

#include "Light.h"

#include <glm/gtc/type_ptr.hpp>

#include "Lumiere/ScriptEngine.h"

namespace lum::comp
{
bool Light::m_registered = false;

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

    m_registered = true;
}

Light::Light(Node3D *node)
    : IComponent(node)
{
    if (m_registered == false)
        RegisterType();
}

void Light::DrawInspector()
{
    static ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen;
    if (ImGui::TreeNodeEx(ICON_FA_LIGHTBULB_O " Light", flags))
    {
        ImGui::Combo("Light type", &m_selectedType, LIGHT_TYPES, LIGHT_TYPE_COUNT);

        ImGui::ColorPicker3("Tint", glm::value_ptr(m_color));
        ImGui::DragFloat("Intensity", &m_intensity, 0.1f);

        // type dependant editor: 0 is directional, 1 point, 2 spot
        if (m_selectedType == 1)
        {
            ImGui::DragFloat("Range", &m_pointRange, 1.f);
        }
        ImGui::TreePop();
    }
}
} // lum::comp