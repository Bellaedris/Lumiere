//
// Created by belle on 25/02/2026.
//

#pragma once
#include "IComponent.h"
#include "Lumiere/Node3D.h"
#include "Lumiere/Graphics/MaterialOverrideBlock.h"
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
    std::vector<gfx::MaterialPtr> m_materials;
    gfx::MaterialOverrideBlock* m_materialOverrideBlock {nullptr};

    /**
     * \brief Registers this component to the Lua scriping backend
     */
    void RegisterType(SystemProvider* systems);

    void InspectorDrawMeshDetails();
public:
    MeshRenderer(Node3D* node, SystemProvider* systems);

    gfx::MeshPtr Mesh() const { return m_mesh; }
    std::vector<gfx::MaterialPtr>& Materials() { return m_materials; }
    gfx::MaterialOverrideBlock* MaterialOverrideBlock() const { return m_materialOverrideBlock; }

    void SetMesh(const std::string& path);
    void SetMaterial(int matIndex, const std::string handle);
    void SetMaterialOverrideBlock(gfx::MaterialOverrideBlock* block) {m_materialOverrideBlock = block;};

    void Serialize(YAML::Node& node) override;
    void Deserialize(YAML::Node& node) override;
};
} // lum::comp