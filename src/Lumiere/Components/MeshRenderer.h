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
    /** \brief registration to the component factory status */
    static bool m_registered;
    /** \brief lua type registration status */
    static bool m_typeRegistered;

    gfx::MeshPtr m_mesh;

    /**
     * \brief Registers this component to the Lua scriping backend
     */
    void RegisterType();

    void InspectorDrawMeshDetails();
public:
    MeshRenderer(Node3D* node);

    gfx::MeshPtr Mesh() const { return m_mesh; }

    void SetMesh(const std::string& path);

    void Serialize(YAML::Node node) override;
    void Deserialize(YAML::Node node) override;
};
} // lum::comp