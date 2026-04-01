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
    // TODO expose RB settings and methods for physics interactions

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
    glm::quat rot = m_node->GetTransform()->LocalRotation();

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

    if (m_motionType == MotionType::Dynamic)
    {
        bodySettings.mMassPropertiesOverride.mMass = m_mass;
        bodySettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
    }

    m_bodyID = m_physicsSystem->BodyInterface()->CreateAndAddBody(
        bodySettings,
        m_motionType == MotionType::Dynamic ? JPH::EActivation::Activate : JPH::EActivation::DontActivate
    );

    if (m_bodyID.IsInvalid())
    {
        std::cerr << "Couldn't create a rigidbody on Node " << m_node->Name() << "\n";
        return;
    }

    // We will need some kind of registration so other rigidbodies can find a Node from a bodyID
    m_physicsSystem->Register(m_bodyID, m_node);
}

void Rigidbody::OnStop()
{
    m_physicsSystem->BodyInterface()->RemoveBody(m_bodyID);
    m_physicsSystem->BodyInterface()->DestroyBody(m_bodyID);
    m_physicsSystem->Unregister(m_bodyID);
}

void Rigidbody::Serialize(YAML::Node node)
{
    YAML::Node s;
    s["componentType"] = "Rigidbody";
    s["motionType"] = m_selectedMotionType;
    s["mass"] = m_mass;
    node.push_back(s);
}

void Rigidbody::Deserialize(YAML::Node node)
{
    SetSelectedMotionType(node["motionType"].as<int>());
    m_mass = node["mass"].as<float>();
}
} // lum