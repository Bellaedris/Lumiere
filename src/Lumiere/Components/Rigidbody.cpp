//
// Created by belle on 18/03/2026.
//

#include "Rigidbody.h"

#include "Collider.h"
#include "Lumiere/Node3D.h"
#include "Lumiere/Systems/PhysicsSystem.h"
#include "Lumiere/Components/Transform.h"
#include "Lumiere/Systems/ScriptEngine.h"

namespace lum::comp
{
bool Rigidbody::m_typeRegistered = false;

REGISTER_TO_COMPONENT_FACTORY(Rigidbody, "Rigidbody");
JPH::EMotionType Rigidbody::LumToJoltMotionType(MotionType type)
{
    switch (type)
    {
        case MotionType::Static:
            return JPH::EMotionType::Static;
        case MotionType::Dynamic:
            return JPH::EMotionType::Dynamic;
        case MotionType::Kinematic:
            return JPH::EMotionType::Kinematic;
        default:
            return JPH::EMotionType::Static;
    }
}

Rigidbody::Rigidbody(Node3D *parent, SystemProvider *systems)
    : IComponent(parent, systems)
    , m_physicsSystem(systems->m_physics)
    , m_scriptEngine(systems->m_scripting)
{
    if (m_typeRegistered == false)
        RegisterTypes();
}

void Rigidbody::RegisterTypes()
{
    m_typeRegistered = true;
    sol::state& lua = m_scriptEngine->State();
    sol::usertype<Rigidbody> rbType = lua.new_usertype<Rigidbody>("Rigidbody");

    rbType["AddForce"] = [](const Rigidbody& rb, const glm::vec3& f) { rb.AddForce(f); };
    rbType["AddForceAtPosition"] = &Rigidbody::AddForceAtPosition;
    rbType["AddImpulse"] = &Rigidbody::AddImpulse;
    rbType["AddImpulseAtPosition"] = &Rigidbody::AddImpulseAtPosition;
    rbType["AddTorque"] = &Rigidbody::AddTorque;
    rbType["AddAngularImpulse"] = &Rigidbody::AddAngularImpulse;
    rbType["SetLinearVelocity"] = [](const Rigidbody& rb, const glm::vec3& f) { rb.SetLinearVelocity(f); };
}

void Rigidbody::SetSelectedMotionType(int motionType)
{
    switch (motionType)
    {
        case 0:
            m_motionType = MotionType::Static;
            break;
        case 1:
            m_motionType = MotionType::Dynamic;
            break;
        case 2:
            m_motionType = MotionType::Kinematic;
            break;
        default:
            std::cerr << "Unknown motion type\n";
            return;
    }
    m_selectedMotionType = motionType;
}

void Rigidbody::AddForce(const glm::vec3 &force) const
{
    m_physicsSystem->BodyInterface()->AddForce(m_bodyID, {force.x, force.y, force.z});
}

void Rigidbody::AddForceAtPosition(const glm::vec3 &force, const glm::vec3 &position) const
{
    m_physicsSystem->BodyInterface()->AddForce(
        m_bodyID,
        {force.x, force.y, force.z},
        {position.x, position.y, position.z}
    );
}

void Rigidbody::AddImpulse(const glm::vec3 &impulse)
{
    m_physicsSystem->BodyInterface()->AddImpulse(m_bodyID, {impulse.x, impulse.y, impulse.z});
}

void Rigidbody::AddImpulseAtPosition(const glm::vec3 &impulse, const glm::vec3 &position)
{
    m_physicsSystem->BodyInterface()->AddImpulse(
        m_bodyID,
        {impulse.x, impulse.y, impulse.z},
        {position.x, position.y, position.z}
    );
}

void Rigidbody::AddTorque(const glm::vec3 &torque)
{
    m_physicsSystem->BodyInterface()->AddTorque(m_bodyID, {torque.x, torque.y, torque.z});
}

void Rigidbody::AddAngularImpulse(const glm::vec3 &impulse)
{
    m_physicsSystem->BodyInterface()->AddAngularImpulse(m_bodyID, {impulse.x, impulse.y, impulse.z});
}

void Rigidbody::SetLinearVelocity(const glm::vec3 &velocity) const
{
    m_physicsSystem->BodyInterface()->SetLinearVelocity(m_bodyID, {velocity.x, velocity.y, velocity.z});
}

void Rigidbody::OnPlay()
{
    // find a rigidbody
    std::optional<Collider*> col = m_node->GetComponent<Collider>();
    if (col.has_value() == false)
    {
        std::cerr << "The node " << m_node->Name() << " has a Rigidbody but no associated collider\n";
        return;
    }

    JPH::ShapeRefC shape = col.value()->CreateShape();
    if (shape == nullptr)
    {
        std::cerr << "Couldn't create collider for node " << m_node->Name() << "\n";
        return;
    }

    glm::vec3 pos = m_node->GetTransform()->Position();
    glm::quat rot = m_node->GetTransform()->Rotation();

    JPH::ObjectLayer layer;
    // eventually add a layer for triggers later
    if (m_motionType == MotionType::Static)
        layer = Layers::NON_MOVING;
    else
        layer = Layers::MOVING;

    JPH::BodyCreationSettings bodySettings(
        shape,
        JPH::RVec3(pos.x, pos.y, pos.z),
        JPH::Quat(rot.x, rot.y, rot.z, rot.w),
        LumToJoltMotionType(m_motionType),
        layer
    );
    bodySettings.mLinearDamping = m_linearDamping;
    bodySettings.mAngularDamping = m_angularDamping;

    if (m_motionType == MotionType::Dynamic)
    {
        bodySettings.mMassPropertiesOverride.mMass = m_mass;
        bodySettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
    }

    m_bodyID = m_physicsSystem->BodyInterface()->CreateAndAddBody(
        bodySettings,
        m_motionType == MotionType::Static ? JPH::EActivation::DontActivate : JPH::EActivation::Activate
    );

    if (m_bodyID.IsInvalid())
    {
        std::cerr << "Couldn't create a rigidbody on Node " << m_node->Name() << "\n";
        return;
    }

    // We will need some kind of registration so other rigidbodies can find a Node from a bodyID
    m_physicsSystem->Register(m_bodyID, this);
}

void Rigidbody::OnStop()
{
    m_physicsSystem->BodyInterface()->RemoveBody(m_bodyID);
    m_physicsSystem->BodyInterface()->DestroyBody(m_bodyID);
    m_physicsSystem->Unregister(m_bodyID);
}

void Rigidbody::Serialize(YAML::Node& node)
{
    YAML::Node s;
    s["componentType"] = "Rigidbody";
    s["motionType"] = m_selectedMotionType;
    s["mass"] = m_mass;
    node.push_back(s);
}

void Rigidbody::Deserialize(YAML::Node& node)
{
    SetSelectedMotionType(node["motionType"].as<int>());
    m_mass = node["mass"].as<float>();
}
} // lum