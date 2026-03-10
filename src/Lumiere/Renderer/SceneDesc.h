//
// Created by Bellaedris on 20/01/2026.
//

#pragma once
#include "Camera.h"
#include "Lights.h"
#include "Lumiere/Node3D.h"
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
    static constexpr int SCENE_DESC_SERIALIZATION_VERSION = 1;
    static constexpr const char* SCENE_FILE_FORMAT = ".lumsc";

    std::unique_ptr<Node3D> m_rootNode;
    std::unique_ptr<LightList> m_lights {std::make_unique<LightList>()};

    Camera* m_mainCamera {nullptr};
public:
    struct RenderInstance
    {
        gfx::MeshPtr mesh;
        glm::mat4 model;
    };

    SceneDesc();

    Node3D *AddNode();
    void SetMainCamera(Camera* camera) {m_mainCamera = camera;};

    Camera* Camera() const {return m_mainCamera;};
    const std::unique_ptr<LightList>& Lights() const { return m_lights; };

    /**
     * \brief Gathers all renderable components from the scene and returns them in an array
     * \return An array containing a description of all renderable components in the scene
     */
    std::vector<RenderInstance> RenderInstances();

    /**
     * \brief Gathers all lights from scene hierarchy and upload their data to the GPU
     */
    void GatherAndUploadLights();

    Node3D* RootNode() {return m_rootNode.get();};

    void ForEachNode(const std::function<void(Node3D* node)>& callback);

    void Serialize();
    void Deserialize(const std::string& path);
};
} // lum::rdr