//
// Created by belle on 18/03/2026.
//

#pragma once
#include "IComponent.h"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <glm/glm.hpp>

namespace lum::comp
{
class Collider : public IComponent
{
private:
    PhysicsSystem* m_physicsSystem {nullptr};

public:
    Collider(Node3D* node, SystemProvider* systems);

    virtual JPH::ShapeRefC CreateShape() = 0;
};
} // lum