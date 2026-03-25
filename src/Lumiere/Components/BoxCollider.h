//
// Created by belle on 19/03/2026.
//

#pragma once
#include "Collider.h"

#include <Jolt/Physics/Collision/Shape/BoxShape.h>

namespace lum::comp
{
class BoxCollider : public Collider
{
private:
    glm::vec3 m_boxSize {1.f, 1.f, 1.f};

    static bool m_registered;
public:
    BoxCollider(Node3D* node, SystemProvider* systems);
    JPH::ShapeRefC CreateShape() override;
    void RegisterGuizmo() override;

    glm::vec3& Size() { return m_boxSize; }

    void Serialize(YAML::Node node) override;
    void Deserialize(YAML::Node node) override;
};
} // lum