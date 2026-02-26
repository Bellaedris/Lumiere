//
// Created by belle on 25/02/2026.
//

#pragma once
#include "IComponent.h"
#include "Lumiere/Graphics/Mesh.h"

namespace lum::comp
{
class MeshRenderer : public IComponent
{
private:
    gfx::MeshPtr m_mesh;
public:
    MeshRenderer();

    gfx::MeshPtr Mesh() const { return m_mesh; }

    void SetMesh(const std::string& path);
};
} // lum::comp