//
// Created by belle on 18/03/2026.
//

#pragma once

namespace lum
{
class ScriptEngine;
class PhysicsSystem;
/**
 * \brief Non-owning pointers to all the systems that will be used by our components during runtime, be it editor or gameplay.
 * For instance, our Rigidbodies need access to our Jolt wrapper in order to create the simulated object.\n
 * This object is then passed to the scene, that will in turn pass it to the Node3Ds, that will onoly pass the relevant
 * members to its components.
 */
struct SystemProvider
{
    PhysicsSystem* m_physics;
    ScriptEngine* m_scripting;
};

class ISystem
{
public:
    virtual void Update(float dt) = 0;
};
}
