//
// Created by belle on 18/03/2026.
//

#pragma once
#include "IComponent.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Math/Quat.h"
#include "glm/vec3.hpp"

namespace lum::comp
{
class Rigidbody : public IComponent
{
public:
    enum class MotionType
    {
        Static,
        Dynamic,
        Kinematic
    };

private:
    static bool m_registered;
    static bool m_typeRegistered;

    PhysicsSystem* m_physicsSystem {nullptr};
    ScriptEngine* m_scriptEngine {nullptr};
    JPH::BodyID m_bodyID;

    MotionType m_motionType {MotionType::Static};
    int m_selectedMotionType {0};

    /**
     * \brief Mass in Kg
     */
    float m_mass {1.f};
    float m_angularDamping {.05f};
    float m_linearDamping {.05f};

    static JPH::EMotionType LumToJoltMotionType(MotionType type);
    /**
     * \brief Registers rigidbody type for the lua VM
     */
    void             RegisterTypes();
public:
    constexpr static std::array<const char*, 3> DISPLAYABLE_MOTION_TYPES = {"Static", "Dynamic", "Kinematic"};

    Rigidbody(Node3D* parent, SystemProvider* systems);

    // Accessors
    float& Mass() { return m_mass; };
    float& LinearDamping() { return m_linearDamping; };
    float& AngularDamping() { return m_angularDamping; };
    MotionType GetMotionType() const {return m_motionType;};
    int SelectedMotionTypeIndex() const { return m_selectedMotionType; }
    const char* SelectedMotionType() const { return DISPLAYABLE_MOTION_TYPES[m_selectedMotionType]; };
    void SetSelectedMotionType(int motionType);

    // state changes
    void AddForce(const glm::vec3& force) const;
    void AddForceAtPosition(const glm::vec3& force, const glm::vec3& position) const;
    void AddImpulse(const glm::vec3& impulse);
    void AddImpulseAtPosition(const glm::vec3& impulse, const glm::vec3& position);
    void AddTorque(const glm::vec3& torque);
    void AddAngularImpulse(const glm::vec3& impulse);
    void SetLinearVelocity(const glm::vec3& velocity) const;

    // state read


    // lifecycle
    void OnPlay() override;
    void OnStop() override;

    // serialize
    void Serialize(YAML::Node& node) override;
    void Deserialize(YAML::Node& node) override;
};
} // lum