//
// Created by belle on 19/03/2026.
//

#include "SphereCollider.h"

#include "Lumiere/Node3D.h"
#include "Lumiere/Utils/YAMLUtils.h"

namespace lum::comp
{
REGISTER_TO_COMPONENT_FACTORY(SphereCollider, "SphereCollider");
SphereCollider::SphereCollider(Node3D* node, SystemProvider *systems)
    : Collider(node, systems)
{
}

JPH::ShapeRefC SphereCollider::CreateShape()
{
    JPH::SphereShapeSettings shape(m_radius);
    JPH::ShapeSettings::ShapeResult s = shape.Create();

    return s.HasError() ? nullptr : s.Get();
}

void SphereCollider::RegisterGuizmo()
{
    m_guizmoHandle = m_debugRenderer->DebugPass()->DrawDebugSphere(
        m_guizmoHandle,
        {.0f, .0f, .0f},
        m_radius,
        {.0f, 1.f, .0f},
        glm::translate(glm::mat4(1.f), m_node->GetTransform()->Position())
    );
}

void SphereCollider::Serialize(YAML::Node node)
{
    YAML::Node s;
    s["componentType"] = "SphereCollider";
    s["radius"] = m_radius;
    node.push_back(s);
}

void SphereCollider::Deserialize(YAML::Node node)
{
    m_radius = node["radius"].as<float>();
}
} // lum