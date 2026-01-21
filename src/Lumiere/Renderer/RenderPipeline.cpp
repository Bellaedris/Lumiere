//
// Created by Bellaedris on 20/01/2026.
//

#include "RenderPipeline.h"

namespace lum::rdr {
RenderPipeline::RenderPipeline()
{
    m_cameraData = std::make_unique<lum::gpu::Buffer>(lum::gpu::Buffer::BufferType::Uniform);
}

void RenderPipeline::Render(const SceneDesc &scene) const
{
    CameraData cameraData = {scene.Camera()->View(), scene.Camera()->Projection()};
    m_cameraData->Write(sizeof(CameraData), &cameraData, lum::gpu::Buffer::DYNAMIC_DRAW);
    m_cameraData->Bind(0);

    for (const auto& pass : m_passes)
        pass->Render(scene);
}

void RenderPipeline::AddPass(IPass *pass)
{
    m_passes.push_back(pass);
}
} // lum::rdr