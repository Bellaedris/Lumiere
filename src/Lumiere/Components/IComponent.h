//
// Created by belle on 24/02/2026.
//

#pragma once

#include <imgui/imgui.h>

namespace lum::comp
{
class IComponent
{
public:
    virtual      ~IComponent() = default;
    virtual void Update() {  };
    virtual void DrawInspector() = 0;
};
}