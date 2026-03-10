//
// Created by belle on 10/03/2026.
//

#pragma once
#include <functional>
#include <iostream>
#include <map>
#include <memory>

namespace lum
{
class Node3D;
}
namespace lum::comp
{
class IComponent;

/**
 * \brief Creation of concrete Components from a string identifier, obtained during the deserialization of a component
 */
class ComponentFactory
{
private:
    inline static std::map<std::string, std::function<std::unique_ptr<IComponent>(Node3D*)>> m_constructors;
public:
    static bool Register(const std::string& typeName, const std::function<std::unique_ptr<IComponent>(Node3D*)>& constructor)
    {
        return m_constructors.emplace(typeName, constructor).second;
    }

    static std::unique_ptr<IComponent> Create(const std::string& type, Node3D* parent)
    {
        if (m_constructors.contains(type))
        {
            return m_constructors[type](parent);
        }

        std::cerr << "Couldn't find pass type " << type << "\n";
        return {};
    }
};
}
