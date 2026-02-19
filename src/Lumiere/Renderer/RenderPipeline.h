//
// Created by Bellaedris on 20/01/2026.
//

#pragma once
#include <vector>

#include "IPass.h"
#include "Lumiere/ProfilerGPU.h"
#include "Lumiere/Events/EventHandler.h"

namespace lum::rdr
{
#define IMGUI_PASS_DEBUG_IMAGE_OPENGL(textureName) ImGui::Image(ResourcesManager::Instance()->GetTexture(textureName)->Handle(), RenderPipeline::DEBUG_DISPLAY_SIZE, ImVec2(0, 1), ImVec2(1, 0));

class RenderPipeline
{
private:
    constexpr static int SERIALIZER_VERSION = 1;

    struct CameraData
    {
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;
        glm::vec3 position;
    };

    std::string m_name;

    std::vector<std::shared_ptr<IPass>> m_passes;
    std::unique_ptr<gpu::Buffer> m_cameraData;
    std::shared_ptr<evt::EventHandler> m_eventHandler;
public:
    constexpr static const char* RENDERED_FRAME_NAME = "RENDERED_FRAME";
    constexpr static ImVec2 DEBUG_DISPLAY_SIZE = ImVec2(200.f, 200.f);

    RenderPipeline(const std::string& name, const std::shared_ptr<evt::EventHandler>& handler);
    void Render(const FrameData &frameData) const;
    void RenderUI();
    void AddPass(const std::shared_ptr<IPass>& pass);

    /**
     * \brief Export the render pipeline and all its passes to a yaml file
     */
    void Serialize() const;

    /**
     * \brief Read a YAML node containing the pipeline description and deserializes all associated passes
     * \param pipeline a YAML node containing the pipeline description
     */
    void Deserialize(const YAML::Node &pipeline);

    /**
     * \brief Rebuild the pipeline to account for a new final resolution size
     * \param newSize new target size
     */
    void Rebuild(const glm::vec2& newSize);

    void OnEvent(const std::shared_ptr<evt::IEvent>& e);
};
} // lum::rdr