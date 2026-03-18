//
// Created by belle on 13/03/2026.
//

#include "PhysicsSystem.h"

namespace lum
{
PhysicsSystem::PhysicsSystem()
{
    JPH::RegisterDefaultAllocator();

    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();

    m_tempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);

    m_broadphaseLayerInterfaceMask = std::make_unique<JPH::BroadPhaseLayerInterfaceMask>(PHYSICS_BROADPHASE_NUM_LAYERS);
    m_objectVsBroadPhaseLayerFilterMask = std::make_unique<JPH::ObjectVsBroadPhaseLayerFilterMask>(*m_broadphaseLayerInterfaceMask);
    m_objectLayerPairFilterMask = std::make_unique<JPH::ObjectLayerPairFilterMask>();

    m_physicsSystem.Init(
            PHYSICS_MAX_BODIES,
            PHYSICS_NUM_BODY_MUTEX,
            PHYSICS_MAX_BODY_PAIRS,
            PHYSICS_MAX_CONTACT_CONSTRAINTS,
            *m_broadphaseLayerInterfaceMask,
            *m_objectVsBroadPhaseLayerFilterMask,
            *m_objectLayerPairFilterMask
    );

    m_bodyInterface = &m_physicsSystem.GetBodyInterface();
}

PhysicsSystem::~PhysicsSystem()
{
    JPH::UnregisterTypes();
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;
}
} // lum