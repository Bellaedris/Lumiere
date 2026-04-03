//
// Created by belle on 13/03/2026.
//

#pragma once

#include <map>
#include <Jolt/Jolt.h>

#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

#include "System.h"
#include "Jolt/Physics/Collision/BroadPhase/ObjectVsBroadPhaseLayerFilterMask.h"
#include "Jolt/Physics/Collision/BroadPhase/BroadPhaseLayerInterfaceMask.h"
#include "Jolt/Physics/Body/BodyInterface.h"
#include "Jolt/Physics/Collision/RayCast.h"
#include "Jolt/Physics/Collision/CastResult.h"
#include "Jolt/Physics/Collision/NarrowPhaseQuery.h"
#include "Lumiere/Components/Rigidbody.h"

#include "glm/glm.hpp"

namespace lum
{
class Node3D;

#pragma region Structs
struct Ray
{
    /**
     * \brief Ray origin in world space
     */
    glm::vec3 origin;

    /**
     * \brief Ray direction. Always normalized by the constructor
     */
    glm::vec3 direction;

    /**
     * \brief Maximal hit distance. Hits with a distance greater than this will be ignored
     */
    float maxHitDistance;
    Ray(const glm::vec3& o, const glm::vec3& d, float hd) : origin(o), direction(glm::normalize(d)), maxHitDistance(hd) {}
};

struct RaycastResult
{
    /**
     * \brief Distance from the ray origin
     */
    float distance;

    /**
     * \brief Node that was hit
     */
    Node3D* node;

    /**
     * \brief World position of the hit
     */
    glm::vec3 position;

    /**
     * \brief Hit normal
     */
    glm::vec3 normal;
};
#pragma endregion // Structs

namespace Layers
{
    static constexpr JPH::ObjectLayer NON_MOVING = 0;
    static constexpr JPH::ObjectLayer MOVING     = 1;
    static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
};

namespace BroadPhaseLayers
{
    static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
    static constexpr JPH::BroadPhaseLayer MOVING(1);
    static constexpr uint                 NUM_LAYERS(2);
};

class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
{
public:
    BPLayerInterfaceImpl()
    {
        // Create a mapping table from object to broad phase layer
        mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
        mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
    }

    virtual uint					GetNumBroadPhaseLayers() const override
    {
        return BroadPhaseLayers::NUM_LAYERS;
    }

    virtual JPH::BroadPhaseLayer			GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
    {
        JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
        return mObjectToBroadPhase[inLayer];
    }

    #if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    virtual const char *			GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
    {
        switch (static_cast<JPH::BroadPhaseLayer::Type>(inLayer))
        {
            case static_cast<JPH::BroadPhaseLayer::Type>(BroadPhaseLayers::NON_MOVING):	return "NON_MOVING";
            case static_cast<JPH::BroadPhaseLayer::Type>(BroadPhaseLayers::MOVING):		return "MOVING";
            default:													JPH_ASSERT(false); return "INVALID";
        }
    }
    #endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
    JPH::BroadPhaseLayer					mObjectToBroadPhase[Layers::NUM_LAYERS];
};

/// Class that determines if an object layer can collide with a broadphase layer
class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
{
public:
    virtual bool				ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
    {
        switch (inLayer1)
        {
            case Layers::NON_MOVING:
                return inLayer2 == BroadPhaseLayers::MOVING;
            case Layers::MOVING:
                return true;
            default:
                JPH_ASSERT(false);
                return false;
        }
    }
};

class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
{
public:
    virtual bool					ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
    {
        switch (inObject1)
        {
            case Layers::NON_MOVING:
                return inObject2 == Layers::MOVING; // Non moving only collides with moving
            case Layers::MOVING:
                return true; // Moving collides with everything
            default:
                JPH_ASSERT(false);
                return false;
        }
    }
};

class PhysicsSystem : public ISystem
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

    constexpr static int MAX_CATCHUP_FRAME = 4;
    #pragma endregion // Constants

    std::unique_ptr<JPH::TempAllocatorImpl> m_tempAllocator {nullptr};
    std::unique_ptr<JPH::JobSystemThreadPool> m_jobSystem {nullptr};

    std::unique_ptr<BPLayerInterfaceImpl> m_broadphaseLayerInterfaceMask {nullptr};
    std::unique_ptr<ObjectVsBroadPhaseLayerFilterImpl> m_objectVsBroadPhaseLayerFilterMask {nullptr};
    std::unique_ptr<ObjectLayerPairFilterImpl> m_objectLayerPairFilterMask {nullptr};

    JPH::PhysicsSystem m_physicsSystem;
    JPH::BodyInterface* m_bodyInterface {nullptr};

    std::map<JPH::BodyID, comp::Rigidbody*> m_bodiesToNodes;

    float m_timeSinceLastPhysicsTick {.0f};
public:
    PhysicsSystem();
    ~PhysicsSystem();

    void Update(float dt);

    std::optional<RaycastResult> Raycast(const Ray& ray);

    JPH::BodyInterface* BodyInterface() { return m_bodyInterface; }

    void Register(JPH::BodyID id, comp::Rigidbody* rb);
    void Unregister(JPH::BodyID body);
};
} // lum