//
// Created by belle on 31/03/2026.
//

#pragma once
#include <map>

#include "sol.hpp"

namespace lum::evt
{
class ScriptEventHandler
{
private:
    std::map<std::string, std::vector<sol::function>> m_listeners;
public:
    void Subscribe(const std::string& name, const sol::function& callback);
    void Emit(const std::string& name, const sol::variadic_args& args);
};
} // lum