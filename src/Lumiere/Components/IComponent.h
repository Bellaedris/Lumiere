//
// Created by belle on 24/02/2026.
//

#pragma once

namespace lum
{
class Node3D;
}

namespace lum::comp
{
class IComponent
{
protected:
    Node3D* m_node{nullptr};

public:
    IComponent(Node3D* node) : m_node(node) {};
    virtual      ~IComponent() = default;
    virtual void Update(float dt) {  };
};
}
