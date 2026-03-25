//
// Created by belle on 18/03/2026.
//

#pragma once
#include "IComponent.h"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <glm/glm.hpp>

#include "Lumiere/RendererManager.h"

namespace lum::comp
{
class Collider : public IComponent
{
protected:
    RendererManager* m_debugRenderer {nullptr};

    rdr::DebugShapeHandle m_guizmoHandle {};
public:
    Collider(Node3D* node, SystemProvider* systems);
    ~Collider() override { Collider::UnregisterGuizmo(); }

    virtual JPH::ShapeRefC CreateShape() = 0;
    void                   RegisterGuizmo() override = 0;
    void                   UnregisterGuizmo() override{ m_debugRenderer->DebugPass()->RemoveShape(m_guizmoHandle); };
};
} // lum