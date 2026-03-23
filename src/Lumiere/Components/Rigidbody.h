//
// Created by belle on 18/03/2026.
//

#pragma once
#include "IComponent.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Math/Quat.h"

namespace lum::comp
{
class Rigidbody : public IComponent
{
private:
    enum class MotionType
    {
        Static,
        Dynamic,
        Kinematic
    };

    PhysicsSystem* m_physicsSystem {nullptr};
    JPH::BodyID m_bodyID;

    MotionType m_motionType {MotionType::Static};
    int m_selectedMotionType {0};

    /**
     * \brief Mass in Kg
     */
    float m_mass {1.f};

    JPH::EMotionType LumToJoltMotionType(MotionType type);

    static bool m_registered;
public:
    constexpr static std::array<const char*, 3> DISPLAYABLE_MOTION_TYPES = {"Static", "Dynamic", "Kinematic"};

    Rigidbody(Node3D* parent, SystemProvider* systems);

    // Accessors
    float& Mass() { return m_mass; };
    int SelectedMotionTypeIndex() const { return m_selectedMotionType; }
    const char* SelectedMotionType() const { return DISPLAYABLE_MOTION_TYPES[m_selectedMotionType]; };
    void SetSelectedMotionType(int motionType);

    // lifecycle
    void OnPlay() override;
    void OnStop() override;

    // serialize
    void Serialize(YAML::Node node) override;
    void Deserialize(YAML::Node node) override;
};
} // lum