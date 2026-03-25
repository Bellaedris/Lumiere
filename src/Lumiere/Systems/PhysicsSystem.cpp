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

void PhysicsSystem::Update(float dt)
{
    // FIXME WE MAY WANT TO MOVE THE CATCHUP PHYSICS TO A HIGHER LEVEL, OTHERWISE WE CAN'T
    // HAVE OTHER CODE PARTS EXECUTING PHYSICS DEPENDANT CODE (FixedUpdate and such that should only be called
    // when the physics refreshes)

    // call OptimizeBroadphase on first update/upon bodies insertion

    m_timeSinceLastPhysicsTick += dt;
    while (m_timeSinceLastPhysicsTick > PHYSICS_DELTA_TIME)
    {
        m_timeSinceLastPhysicsTick -= PHYSICS_DELTA_TIME;
        // Note: in case of physics catchup, we might run more than 1 collision steps, or 0.
        JPH::EPhysicsUpdateError updateRes       = m_physicsSystem.Update(
                PHYSICS_DELTA_TIME,
                1,
                m_tempAllocator.get(),
                m_jobSystem.get());

        if (updateRes != JPH::EPhysicsUpdateError::None)
        {
            std::cerr << "Physics update error\n";
        }

        // after the physics update, sync the positions with the gameObjects
        for (auto& body : m_bodiesToNodes)
        {
            comp::Transform* t = body.second->GetTransform();

            JPH::RVec3 jPos;
            JPH::Quat  jRot;
            m_bodyInterface->GetPositionAndRotation(body.first, jPos, jRot);

            t->SetPosition({jPos.GetX(), jPos.GetY(), jPos.GetZ()});
            t->SetRotation({jRot.GetX(), jRot.GetY(), jRot.GetZ(), jRot.GetW()});
        }
    }
}

std::optional<RaycastResult> PhysicsSystem::Raycast(const Ray &ray)
{
    // jolt's raycast has the max hit distance embedded in the ray direction
    JPH::RRayCast r(
        {ray.origin.x, ray.origin.y, ray.origin.z},
        {ray.direction.x * ray.maxHitDistance, ray.direction.y * ray.maxHitDistance, ray.direction.z * ray.maxHitDistance}
    );

    JPH::RayCastResult res;
    // TODO allow the passing of layers to filter the collisions, when we have actual layer add/delete to work with
    if (m_physicsSystem.GetNarrowPhaseQuery().CastRay(r, res) == false)
        return {};

    JPH::Vec3 normal;
    const JPH::BodyLockInterfaceLocking& lockInterface = m_physicsSystem.GetBodyLockInterface();
    {
        JPH::BodyLockRead lock(lockInterface, res.mBodyID);
        if (lock.Succeeded())
        {
            const JPH::Body &body = lock.GetBody();
            normal = body.GetWorldSpaceSurfaceNormal(res.mSubShapeID2, r.GetPointOnRay(res.mFraction));
        }
    }

    RaycastResult result {
        .distance = res.mFraction * ray.maxHitDistance,
        .node = m_bodiesToNodes[res.mBodyID],
        .position = ray.origin + res.mFraction * ray.maxHitDistance * ray.direction,
        .normal = {normal.GetX(), normal.GetY(), normal.GetZ()}
    };

    return {std::move(result)};
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