//
// Created by belle on 25/02/2026.
//

#pragma once
#include "IComponent.h"
#include "Lumiere/Graphics/Mesh.h"

#include <imgui/imfilebrowser.h>

namespace lum::comp
{
class MeshRenderer : public IComponent
{
private:
    ImGui::FileBrowser m_fileBrowser;
    gfx::MeshPtr m_mesh;
public:
    MeshRenderer();
    void DrawInspector() override;

    gfx::MeshPtr Mesh() const { return m_mesh; }
};
} // lum::comp