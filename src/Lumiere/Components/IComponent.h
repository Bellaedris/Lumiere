//
// Created by belle on 24/02/2026.
//

#pragma once

namespace lum::comp
{
class IComponent
{
public:
    virtual      ~IComponent() = default;
    virtual void Update() {  };
};
}