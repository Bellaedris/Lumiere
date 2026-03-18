//
// Created by belle on 18/03/2026.
//

#pragma once
#include "IComponent.h"

namespace lum::comp
{
class Rigidbody : public IComponent
{
private:
    PhysicsSystem* m_physicsSystem {nullptr};

public:
    Rigidbody(Node3D* parent, SystemProvider* systems);

    void Serialize(YAML::Node node) override;
    void Deserialize(YAML::Node node) override;
};
} // lum