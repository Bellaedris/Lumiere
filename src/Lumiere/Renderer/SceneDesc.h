//
// Created by Bellaedris on 20/01/2026.
//

#pragma once
#include "Camera.h"
#include "Lights.h"
#include "Lumiere/Graphics/Mesh.h"

namespace lum::rdr
{
/**
 * \brief A renderer-side description of a scene. Should be decorelated from the editor scene, as it only serves as a reference
 * of what we should use to draw the next frame
 */
class SceneDesc
{
protected:
    std::vector<gfx::MeshPtr> m_meshes {};
    std::unique_ptr<LightList> m_lights {std::make_unique<LightList>()};

    Camera* m_mainCamera {nullptr};
public:
    void AddMesh(const std::string &path);
    void SetMainCamera(Camera* camera) {m_mainCamera = camera;};

    Camera* Camera() const {return m_mainCamera;};
    const std::vector<gfx::MeshPtr>& Meshes() const { return m_meshes; };
    const std::unique_ptr<LightList>& Lights() const { return m_lights; };

    /**
     * \brief Just a lambda to do things on all submeshes of a scene. Saves me a few double for loops,
     * thank you https://github.com/AmelieHeinrich/Seraph/blob/main/code/Renderer/Hybrid/SP_GBuffer.cpp !
     * \param callback lambda called with every SubMesh/material pair of every Mesh of the scene.
     */
    void ForEach(const std::function<void(const gfx::SubMesh &primitive, const gfx::MaterialPtr &material)> &callback) const;
};
} // lum::rdr