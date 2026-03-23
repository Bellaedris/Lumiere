//
// Created by belle on 19/03/2026.
//

#include "BoxCollider.h"

#include "Lumiere/Node3D.h"
#include "Lumiere/Utils/YAMLUtils.h"

namespace lum::comp
{
REGISTER_TO_COMPONENT_FACTORY(BoxCollider, "BoxCollider");
BoxCollider::BoxCollider(Node3D* node, SystemProvider *systems)
    : Collider(node, systems)
{
}

JPH::ShapeRefC BoxCollider::CreateShape()
{
    JPH::BoxShapeSettings shape({m_boxSize.x, m_boxSize.y, m_boxSize.z});
    JPH::ShapeSettings::ShapeResult s = shape.Create();

    return s.HasError() ? nullptr : s.Get();
}

void BoxCollider::Serialize(YAML::Node node)
{
    YAML::Node s;
    s["componentType"] = "BoxCollider";
    s["size"] = m_boxSize;
    node.push_back(s);
}

void BoxCollider::Deserialize(YAML::Node node)
{
    m_boxSize = node["size"].as<glm::vec3>();
}
} // lum