//
// Created by belle on 13/03/2026.
//

#include "PhysicsSystem.h"

#include <iostream>

#include "Lumiere/Node3D.h"
#include "Lumiere/Components/Transform.h"

namespace lum
{
PhysicsSystem::PhysicsSystem()
{
    JPH::RegisterDefaultAllocator();

    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();

    m_tempAllocator = std::make_unique<JPH::TempAllocatorImpl>(10 * 1024 * 1024);

    m_jobSystem = std::make_unique<JPH::JobSystemThreadPool>(
            JPH::cMaxPhysicsJobs,
            JPH::cMaxPhysicsBarriers,
            std::max(1, static_cast<int>(std::thread::hardware_concurrency()) - 1));

    m_broadphaseLayerInterfaceMask = std::make_unique<BPLayerInterfaceImpl>();
    m_objectVsBroadPhaseLayerFilterMask = std::make_unique<ObjectVsBroadPhaseLayerFilterImpl>();
    m_objectLayerPairFilterMask = std::make_unique<ObjectLayerPairFilterImpl>();

    m_physicsSystem.Init(
            PHYSICS_MAX_BODIES,
            PHYSICS_NUM_BODY_MUTEX,
            PHYSICS_MAX_BODY_PAIRS,
            PHYSICS_MAX_CONTACT_CONSTRAINTS,
            *m_broadphaseLayerInterfaceMask,
            *m_objectVsBroadPhaseLayerFilterMask,
            *m_objectLayerPairFilterMask
    );
    m_physicsSystem.SetGravity({.0f, -9.81f, .0f});

    m_bodyInterface = &m_physicsSystem.GetBodyInterface();
}

PhysicsSystem::~PhysicsSystem()
{
    JPH::UnregisterTypes();
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;
}

void PhysicsSystem::Update()
{
    // call OptimizeBroadphase on first update/upon bodies insertion

    // Note: in case of physics catchup, we might run more than 1 collision steps, or 0.
    int                      collisionSteps = 1;
    JPH::EPhysicsUpdateError updateRes       = m_physicsSystem.Update(
            PHYSICS_DELTA_TIME,
            collisionSteps,
            m_tempAllocator.get(),
            m_jobSystem.get());

    if (updateRes != JPH::EPhysicsUpdateError::None)
    {
        std::cerr << "Physics update error\n";
    }

    // after the physics update, sync the positions with the gameObjects
    for (auto& body : m_bodiesToNodes)
    {
        comp::Transform& t = body.second->GetTransform();

        JPH::RVec3 jPos;
        JPH::Quat  jRot;
        m_bodyInterface->GetPositionAndRotation(body.first, jPos, jRot);

        t.SetPosition({jPos.GetX(), jPos.GetY(), jPos.GetZ()});
        t.SetRotation({jRot.GetX(), jRot.GetY(), jRot.GetZ(), jRot.GetW()});
    }
}

void PhysicsSystem::Register(JPH::BodyID id, Node3D *node)
{
    m_bodiesToNodes.emplace(id, node);
}

void PhysicsSystem::Unregister(JPH::BodyID body)
{
    m_bodiesToNodes.erase(body);
}
} // lum