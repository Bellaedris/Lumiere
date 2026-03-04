//
// Created by belle on 25/02/2026.
//

#include "MeshRenderer.h"

#include "Lumiere/Utils/MeshLoader.h"

namespace lum::comp
{

void MeshRenderer::SetMesh(const std::string &path)
{
    utils::MeshLoader loader;
    m_mesh = loader.LoadMeshFromFile(path);
}

} // lum::comp