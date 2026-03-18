//
// Created by belle on 18/03/2026.
//

#include "Rigidbody.h"

namespace lum::comp
{

Rigidbody::Rigidbody(Node3D *parent, SystemProvider *systems)
    : IComponent(parent, systems)
    , m_physicsSystem(systems->m_physics)
{

}
} // lum