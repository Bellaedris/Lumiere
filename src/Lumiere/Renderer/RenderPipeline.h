//
// Created by Bellaedris on 20/01/2026.
//

#pragma once
#include <vector>

#include "IPass.h"
#include "Lumiere/Events/EventHandler.h"

namespace lum::rdr
{
class RenderPipeline
{
private:
    struct CameraData
    {
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;
    };

    std::vector<IPass*> m_passes;
    std::unique_ptr<gpu::Buffer> m_cameraData;
    std::shared_ptr<evt::EventHandler> m_eventHandler;
public:
    constexpr static const char* RENDERED_FRAME_NAME = "RENDERED_FRAME_NAME";

    RenderPipeline(const std::shared_ptr<evt::EventHandler>& handler);
    void Render(const SceneDesc &scene) const;
    void AddPass(IPass* pass);

    /**
     * \brief Rebuild the pipeline to account for a new final resolution size
     * \param newSize new target size
     */
    void Rebuild(const glm::vec2& newSize);

    void OnEvent(const std::shared_ptr<evt::IEvent>& e);
};
} // lum::rdr