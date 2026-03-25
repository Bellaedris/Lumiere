//
// Created by belle on 18/03/2026.
//

#include "Collider.h"

namespace lum::comp
{
Collider::Collider(Node3D *node, SystemProvider *systems)
    : IComponent(node, systems)
    , m_debugRenderer(systems->m_renderer)
{
}
} // lum