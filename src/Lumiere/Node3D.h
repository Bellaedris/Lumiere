//
// Created by belle on 23/02/2026.
//

#pragma once
#include <memory>

#include "sol.hpp"
#include "Components/Transform.h"
#include "stduuid/uuid.h"

namespace lum
{
class Node3D
{
private:
    static bool m_registered;

    // eventually use an std list if we want easy reordering of childs
    std::vector<std::unique_ptr<Node3D>> m_children;
    Node3D*                              m_parent {nullptr};
    comp::Transform                      m_transform;

    std::vector<std::unique_ptr<comp::IComponent>> m_components;

    std::string m_name {"Node3D"};
    uuids::uuid m_uuid;
    /**
     * \brief We track the depth of each node for utility. Root node has 0, root childs have 1, and so on
     */
    int         m_depth {0};

public:
    constexpr static const size_t NODE_NAME_MAX_LENGTH = 128;

    Node3D();
    Node3D(std::string name);

    Node3D *AddChild();
    void AddChild(std::unique_ptr<Node3D>& child);
    void RemoveChild(Node3D* child);
    void TransferChild(Node3D* child, Node3D* destination);
    void Update(float dt);
    void UpdateSelfAndChildren(float dt);

    // should probably be private, with IComponents being friend classes...
    void SetScriptingContext(sol::environment& env) const;

    template<typename T>
    void AddComponent();
    template<typename T>
    bool HasComponent() const;
    template<typename T>
    std::optional<T*> GetComponent();

    /**
     * \brief Checks if a node has another node as one of its upper level parent. Can be used to avoid moving a node into
     * a node who is part of the hierarchy of the moved node
     * \param node Node to check for parentality
     * \return true if the node is an upper level parent, false otherwise
     */
    bool HasAncestor(Node3D* node) const;
    [[nodiscard]] bool HasChild() const { return m_children.empty() == false; }

    void SetName(const std::string& name) { m_name = name; }

    [[nodiscard]] const std::vector<std::unique_ptr<Node3D>>&Children() const { return m_children; }
    [[nodiscard]] comp::Transform&                           GetTransform() { return m_transform; }
    [[nodiscard]] Node3D*                                    Parent() const { return m_parent; }
    [[nodiscard]] const std::string&                         GetName() const { return m_name; }
    [[nodiscard]] std::string&                               Name() { return m_name; }
    [[nodiscard]] const uuids::uuid&                         UUID() const { return m_uuid; }
    [[nodiscard]] int                                        Depth() const { return m_depth; }
    [[nodiscard]] const std::vector<std::unique_ptr<comp::IComponent>>& Components() const { return m_components; }
};

template<typename T>
void Node3D::AddComponent()
{
    if (HasComponent<T>() == false)
        m_components.push_back(std::make_unique<T>(this));
}

template<typename T>
bool Node3D::HasComponent() const
{
    for (auto&& component : m_components)
    {
        T* res = dynamic_cast<T*>(component.get());
        if (res != nullptr)
        {
            return true;
        }
    }

    return false;
}

template<typename T>
std::optional<T*> Node3D::GetComponent()
{
    for (auto&& component : m_components)
    {
        T* res = dynamic_cast<T*>(component.get());
        if (res != nullptr)
        {
            return res;
        }
    }

    return {};
}
} // lum