//
// Created by belle on 19/03/2026.
//

#pragma once
#include "Collider.h"

#include "Jolt/Physics/Collision/Shape/SphereShape.h"

namespace lum::comp
{
class SphereCollider : public Collider
{
private:
    float m_radius {1.f};

    static bool m_registered;
public:
    SphereCollider(Node3D* node, SystemProvider* systems);
    JPH::ShapeRefC CreateShape() override;

    float& Radius() { return m_radius; }

    void           Serialize(YAML::Node node) override;
    void           Deserialize(YAML::Node node) override;
};
} // lum