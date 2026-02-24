//
// Created by belle on 23/02/2026.
//

#pragma once
#include <vector>

#include "Renderer/RenderPipeline.h"
#include "Renderer/Passes/GBuffer.h"

namespace lum
{
/**
 * \brief Manages multiple renderers and a final render target shared between these renderers
 */
class RendererManager
{
private:
    struct CameraData
    {
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;
        glm::vec3 position;
    };

    std::shared_ptr<evt::EventHandler> m_eventHandler;
    std::vector<rdr::RenderPipeline> m_pipelines;
    // the GBuffer pass has a special status: every pipeline we have is deferred and share a single GBuffer.
    // this looks like very bad architecture tho if i'm being honest.. I'll need to rethink rendering at some point
    std::unique_ptr<rdr::GBuffer> m_gBuffer;

    std::unique_ptr<gpu::Buffer> m_cameraData;
    int m_activePipeline {0};

public:
    constexpr static const char* RENDERED_FRAME_NAME = "RENDERED_FRAME";

    RendererManager(int width, int height, const std::shared_ptr<evt::EventHandler> &eventHandler);
    void AddPipeline(rdr::RenderPipeline& pipeline);
    void SetActivePipeline(int pipelineId);

    void Render(const rdr::FrameData& frameData);
    void RenderUI();

    void OnEvent(const std::shared_ptr<evt::IEvent>& e);

    void Serialize();
};
} // lum

