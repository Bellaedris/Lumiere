//
// Created by belle on 13/03/2026.
//

#pragma once

#include <Jolt/Jolt.h>

#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include "Jolt/Physics/Collision/BroadPhase/ObjectVsBroadPhaseLayerFilterMask.h"
#include "Jolt/Physics/Collision/BroadPhase/BroadPhaseLayerInterfaceMask.h"
#include "Jolt/Physics/Body/BodyInterface.h"

namespace lum
{

class PhysicsSystem
{
private:
    #pragma region Constants
    // 65536 max physics body in the simulation
    constexpr static uint16_t PHYSICS_MAX_BODIES = std::numeric_limits<uint16_t>::max();

    // This determines how many mutexes to allocate to protect rigid bodies from concurrent access. 0 is the default setting
    constexpr static uint PHYSICS_NUM_BODY_MUTEX = 0;

    // 65536 max concurrent body pairs during broadphase
    constexpr static uint16_t PHYSICS_MAX_BODY_PAIRS = std::numeric_limits<uint16_t>::max();

    // max number of collisions in a single frame
    constexpr static uint16_t PHYSICS_MAX_CONTACT_CONSTRAINTS = 10000;

    constexpr static uint16_t PHYSICS_BROADPHASE_NUM_LAYERS = 1;

    // physics refreshes at 60hz
    constexpr static float PHYSICS_DELTA_TIME = 1.f / 60.f;
    #pragma endregion // Constants

    JPH::TempAllocatorImpl* m_tempAllocator {nullptr};
    std::unique_ptr<JPH::BroadPhaseLayerInterfaceMask> m_broadphaseLayerInterfaceMask {nullptr};
    std::unique_ptr<JPH::ObjectVsBroadPhaseLayerFilterMask> m_objectVsBroadPhaseLayerFilterMask {nullptr};
    std::unique_ptr<JPH::ObjectLayerPairFilterMask> m_objectLayerPairFilterMask {nullptr};

    JPH::PhysicsSystem m_physicsSystem;
    JPH::BodyInterface* m_bodyInterface {nullptr};

public:
    PhysicsSystem();
    ~PhysicsSystem();
};
} // lum