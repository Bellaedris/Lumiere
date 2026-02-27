//
// Created by belle on 23/02/2026.
//

#include "Node3D.h"
#include <stduuid/uuid.h>

#include <utility>

namespace lum
{
Node3D::Node3D()
    : m_uuid(uuids::uuid_system_generator{}())
{

}

Node3D::Node3D(std::string name)
    : m_name(std::move(name))
    , m_uuid(uuids::uuid_system_generator{}())
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

void Node3D::Update()
{
    if (m_transform.IsDirty())
    {
        UpdateSelfAndChildren();
        return;
    }

    for (auto&& child : m_children)
        child->Update();
}

void Node3D::UpdateSelfAndChildren()
{
    // if this node is dirty, update this node and its children. Otherwise, just go deeper and update the dirty nodes
    if (m_parent == nullptr)
        m_transform.UpdateModelMatrix();
    else
        m_transform.UpdateModelMatrix(m_parent->GetTransform().Model());

    for (auto&& child : m_children)
        child->UpdateSelfAndChildren();
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