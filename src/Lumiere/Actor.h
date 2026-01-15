//
// Created by Bellaedris on 15/01/2026.
//

#pragma once
#include <string>

#include "Graphics/Mesh.h"

namespace lum
{
class Actor
{
private:
    std::vector<gfx::MeshPtr> m_meshes;
public:
    explicit Actor(const std::string& path);

    void Draw() const;
};
} // lum