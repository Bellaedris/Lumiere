//
// Created by belle on 28/04/2026.
//

#pragma once

#include <functional>
#include <iostream>
#include <map>
#include <memory>


namespace lum::gfx
{
class IMaterial;
/**
 * \brief Creation of concrete Materials from a string identifier, obtained during the deserialization of a material
 */
class MaterialFactory
{
private:
    inline static std::map<std::string, std::function<std::shared_ptr<IMaterial>(const std::string& name)>> m_constructors;
public:
    static bool Register(const std::string& typeName, const std::function<std::shared_ptr<IMaterial>(const std::string& name)>& constructor)
    {
        return m_constructors.emplace(typeName, constructor).second;
    }

    static std::shared_ptr<IMaterial> Create(const std::string& type, const std::string& name)
    {
        if (m_constructors.contains(type))
        {
            return m_constructors[type](name);
        }

        std::cerr << "Couldn't find material type " << type << "\n";
        return {};
    }
};
}
