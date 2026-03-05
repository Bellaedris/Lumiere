//
// Created by belle on 25/02/2026.
//

#pragma once
#include "IComponent.h"
#include "Lumiere/Node3D.h"
#include "Lumiere/Graphics/Mesh.h"

namespace lum::comp
{
class MeshRenderer : public IComponent
{
private:
    static bool m_registered;

    gfx::MeshPtr m_mesh;

    /**
     * \brief Registers this component to the Lua scriping backend
     */
    void RegisterType();
public:
    MeshRenderer(Node3D* node);

    gfx::MeshPtr Mesh() const { return m_mesh; }

    void SetMesh(const std::string& path);
};
} // lum::comp