//
// Created by belle on 24/02/2026.
//

#pragma once
#include "yaml-cpp/yaml.h"

#include <Lumiere/Components/ComponentFactory.h>

namespace lum
{
class Node3D;
}

namespace lum::comp
{
class IComponent
{
#define REGISTER_TO_COMPONENT_FACTORY(type, name) bool type::m_registered = ComponentFactory::Register(name, [](Node3D* p) { return std::make_unique<type>(p); } );

protected:
    Node3D* m_node{nullptr};

public:
    IComponent(Node3D* node) : m_node(node) {};
    virtual      ~IComponent() = default;
    virtual void Update(float dt) {  };

    virtual void Serialize(YAML::Node node) = 0;
    virtual void Deserialize(YAML::Node node) = 0;
};
}
