//
// Created by Bellaedris on 18/02/2026.
//

#pragma once
#include <functional>
#include <map>
#include <string>
#include <memory>

namespace lum::rdr
{
class IPass;
class PassFactory
{
    inline static std::map<std::string, std::function<std::unique_ptr<IPass>()>> m_constructors;

public:
    static bool Register(const std::string& type, const std::function<std::unique_ptr<IPass>()>& constructor)
    {
        return m_constructors.emplace(type, constructor).second;
    }

    static std::unique_ptr<IPass> Create(const std::string& type)
    {
        if (m_constructors.contains(type))
        {
            return m_constructors[type]();
        }

        std::cerr << "Couldn't find pass type " << type << "\n";
        return {};
    }
};
}
