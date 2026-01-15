//
// Created by Bellaedris on 15/01/2026.
//

#pragma once
#include <string>

#include "Mesh.h"

namespace lum
{
class Actor
{
private:
    std::vector<MeshPtr> meshes;
public:
    explicit Actor(const std::string& path);

    void Draw() const;
};
} // lum