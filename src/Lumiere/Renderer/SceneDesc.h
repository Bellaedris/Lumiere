//
// Created by Bellaedris on 20/01/2026.
//

#pragma once
#include "Camera.h"
#include "Lights.h"
#include "Lumiere/App.h"
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

    SystemProvider* m_systemProvider;
    std::unique_ptr<Node3D> m_rootNode;
    std::unique_ptr<LightList> m_lights {std::make_unique<LightList>()};
    bool m_isPlaying {false};

    Camera* m_mainCamera {nullptr};
public:
    struct RenderInstance
    {
        gfx::MeshPtr mesh;
        glm::mat4 model;
    };

    SceneDesc(SystemProvider* systemProvider);

    Node3D *AddNode();
    void SetMainCamera(Camera* camera) {m_mainCamera = camera;};

    bool IsPlaying() const { return m_isPlaying; };
    Camera* MainCamera() const {return m_mainCamera;};
    const std::unique_ptr<LightList>& Lights() const { return m_lights; };

    /**
     * \brief Gathers all renderable components from the scene and returns them in an array
     * \return An array containing a description of all renderable components in the scene
     */
    std::vector<RenderInstance> RenderInstances();

    /**
     * \brief Gathers all renderable UI components from the scene and returns them in an array
     * \return An array containing a description of all renderable UI components in the scene
     */
    std::vector<RenderInstance> UIRenderInstances();

    /**
     * \brief Gathers all lights from scene hierarchy and upload their data to the GPU
     */
    void GatherAndUploadLights();

    Node3D* RootNode() {return m_rootNode.get();};

    /**
     * \brief Delete all nodes
     */
    void ClearNodes();

    /**
     * \brief Saves the current scene and removes all nodes, creating a new empty scene
     */
    void ResetScene();

    /**
     * \brief Serializes the scene and loads another scene right after
     * \param path path to the scene to load
     */
    void LoadScene(const std::string& path);

    void ForEachNode(const std::function<void(Node3D* node)>& callback);

    /**
     * \brief Try to find the first component of a specified type
     * \tparam T A component type
     * \return the first found component of type T, nullptr if none is present
     */
    template<typename T>
    T* FindComponentOfType();

    // lifecycle
    void OnPlay();
    void OnStop();

    /**
     * \brief Serializes the scene to YAML
     * \return The relative path the scene was written to. Something like Assets/name.lumsc. Can be nullopt if the serialization
     * process failed.
     */
    std::optional<std::string> Serialize();
    void Deserialize(const std::string& path);
};

template<typename T>
T * SceneDesc::FindComponentOfType()
{
    T* res = nullptr;
    ForEachNode([&](Node3D* node)
    {
        if (node->HasComponent<T>() && res == nullptr)
            res = node->GetComponent<T>().value();
    });
    return res;
}
} // lum::rdr