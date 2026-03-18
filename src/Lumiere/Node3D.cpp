//
// Created by belle on 23/02/2026.
//

#include "Node3D.h"
#include <stduuid/uuid.h>

#include <utility>

#include "Components/Light.h"
#include "Components/MeshRenderer.h"

namespace lum
{
bool Node3D::m_registered = false;

void Node3D::RegisterType()
{
    m_registered = true;

    sol::state& lua = m_systemProvider->m_scripting->State();
    // since Transforms are always created alongside Nodes, we register the 2 types at the same time so we don't have to
    // give any scripting context to Transform post-creation, which would be very ugly.

    sol::usertype<glm::vec3> vec = lua.new_usertype<glm::vec3>("vec3",
            sol::call_constructor,
            sol::constructors<glm::vec3(float), glm::vec3(float, float, float)>()
        );
    vec["x"] = sol::property(
        [](const glm::vec3& v) { return v.x; },
        [](glm::vec3& v, float x) { v.x = x; }
    );
    vec["y"] = sol::property(
        [](const glm::vec3& v) { return v.y; },
        [](glm::vec3& v, float y) { v.y = y; }
    );
    vec["z"] = sol::property(
        [](const glm::vec3& v) { return v.z; },
        [](glm::vec3& v, float z) { v.z = z; }
    );

    sol::usertype<comp::Transform> transform = lua.new_usertype<comp::Transform>("Transform");

    transform["position"] = sol::property(
        [](comp::Transform& t) -> glm::vec3& { return t.m_position; },
        [](comp::Transform& t, const glm::vec3& p) { t.SetLocalPosition(p); }
    );
    transform["Translate"] = &comp::Transform::Translate;

    transform["rotation"] = sol::property(
        [](comp::Transform& t) -> glm::vec3& { return t.m_rotationEuler; },
        [](comp::Transform& t, const glm::vec3& p) { t.m_rotationEuler = p; }
    );

    transform["scale"] = sol::property(
        [](comp::Transform& t) -> glm::vec3& { return t.m_scale; },
        [](comp::Transform& t, const glm::vec3& p) { t.m_scale = p; }
    );

    transform["worldPosition"] = &comp::Transform::Position;
    transform["worldScale"] = &comp::Transform::Scale;

    sol::usertype<Node3D> type = lua.new_usertype<Node3D>("Node3D");
    // there is no templated functions in lua so we have to bind all the possible get/add components functions by hand :(
    // LAMBDAS MUST RETURN REFERENCES
    type["transform"] = sol::property([](Node3D& node) -> comp::Transform& { return node.m_transform; });
    type["GetMeshComponent"] = &Node3D::GetComponent<comp::MeshRenderer>;
    type["GetLightComponent"] = &Node3D::GetComponent<comp::Light>;

    type["name"] = &Node3D::GetName;
}

Node3D::Node3D()
    : m_transform(this, nullptr)
    , m_uuid(uuids::uuid_system_generator{}())
{

}

Node3D::Node3D(const std::string& name, uuids::uuid& uuid)
    : m_transform(this, nullptr)
    , m_name(name)
    , m_uuid(std::move(uuid))
{

}

Node3D *Node3D::AddChild()
{
    auto child = std::make_unique<Node3D>();
    child->m_parent = this;
    child->m_depth = m_depth + 1;
    m_children.push_back(std::move(child));
    return m_children.back().get();
}

void Node3D::AddChild(std::unique_ptr<Node3D>& child)
{
    child->m_parent = this;
    child->m_depth = m_depth + 1;
    m_children.push_back(std::move(child));
}

void Node3D::RemoveChild(Node3D* child)
{
    auto iter = std::find_if(m_children.begin(), m_children.end(), [&child](const std::unique_ptr<Node3D>& node)
    {
        return node.get() == child;
    });
    if (iter != m_children.end())
    {
        m_children.erase(iter);
    }
}

void Node3D::TransferChild(Node3D *child, Node3D* destination)
{
    auto iter = std::find_if(m_children.begin(), m_children.end(), [&child](const std::unique_ptr<Node3D>& node)
    {
        return node.get() == child;
    });
    if (iter != m_children.end())
    {
        destination->AddChild(*iter);
        m_children.erase(iter);
    }
}

void Node3D::Update(float dt)
{
    if (m_transform.IsDirty())
    {
        UpdateSelfAndChildren(dt);
        return;
    }

    for (auto&& child : m_children)
        child->Update(dt);
}

void Node3D::UpdateSelfAndChildren(float dt)
{
    // if this node is dirty, update this node and its children. Otherwise, just go deeper and update the dirty nodes
    if (m_parent == nullptr)
        m_transform.UpdateModelMatrix();
    else
        m_transform.UpdateModelMatrix(m_parent->GetTransform().Model());

    for (auto&& child : m_children)
        child->UpdateSelfAndChildren(dt);
}

void Node3D::SetSystemsContext(SystemProvider *systemProvider)
{
    m_systemProvider = systemProvider;
    if (m_registered == false)
        RegisterType();
}

void Node3D::AddComponent(std::unique_ptr<comp::IComponent> &component)
{
    m_components.push_back(std::move(component));
}

void Node3D::RemoveComponent(comp::IComponent *component)
{
    auto iter = std::find_if(m_components.begin(), m_components.end(), [&component](const std::unique_ptr<comp::IComponent>& comp)
    {
        return comp.get() == component;
    });
    if (iter != m_components.end())
    {
        m_components.erase(iter);
    }
}

bool Node3D::HasAncestor(Node3D *node) const
{
    const Node3D* current = this;
    while (current->m_parent != nullptr)
    {
        if (current->m_parent == node)
            return true;
        current = current->m_parent;
    }

    return false;
}
} // lum