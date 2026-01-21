//
// Created by Bellaedris on 20/01/2026.
//

#pragma once
#include <vector>

#include "IPass.h"

namespace lum::rdr
{
class RenderPipeline
{
    struct CameraData
    {
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;
    };

private:
    std::vector<IPass*> m_passes;

    std::unique_ptr<lum::gpu::Buffer> m_cameraData;
public:
    RenderPipeline();
    void Render(const SceneDesc &scene) const;
    void AddPass(IPass* pass);
};
} // lum::rdr