//
// Created by belle on 23/02/2026.
//

#include "Node3D.h"
#include <stduuid/uuid.h>

#include <utility>

#include "ScriptEngine.h"
#include "sol.hpp"

namespace lum
{
bool Node3D::m_registered = false;

Node3D::Node3D()
    : m_transform(this)
    , m_uuid(uuids::uuid_system_generator{}())
{
    if (m_registered == false)
    {
        m_registered = true;

        sol::state& lua = ScriptEngine::Instance();
        sol::usertype<Node3D> type = lua.new_usertype<Node3D>("Node3D");

        // there is no templated functions in lua so we have to bind all the possible get/add components functions by hand :(
        // LAMBDAS MUST RETURN REFERENCES
        type["transform"] = sol::property([](Node3D& node) -> comp::Transform& { return node.m_transform; });

        type["name"] = &Node3D::GetName;
    }
}

Node3D::Node3D(std::string name)
    : m_name(std::move(name))
    , m_uuid(uuids::uuid_system_generator{}())
    , m_transform(this)
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

    for (auto&& component : m_components)
        component->Update(dt);
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

    for (auto&& component : m_components)
        component->Update(dt);
    for (auto&& child : m_children)
        child->UpdateSelfAndChildren(dt);
}

void Node3D::SetScriptingContext(sol::environment &env) const
{
    env["node"] = this;
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