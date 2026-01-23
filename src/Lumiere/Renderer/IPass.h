//
// Created by Bellaedris on 20/01/2026.
//

#pragma once
#include "SceneDesc.h"
#include "Lumiere/GPU/Framebuffer.h"

namespace lum::rdr
{
/**
 * \brief A pass is a framebuffer that will be drawn by reading a scene and its parameters
 */
class IPass
{
protected:
    /** \brief a pass may have a frame buffer, but it is not mandatory (for a compute-only pass, for instance) */
    std::unique_ptr<gpu::Framebuffer> m_framebuffer {nullptr};
public:
    virtual ~IPass() = default;

    IPass() : m_framebuffer(nullptr) {}
    IPass(uint32_t width, uint32_t height) : m_framebuffer(std::make_unique<gpu::Framebuffer>(width, height)) {}
    virtual void Render(const SceneDesc &scene) = 0;
};
} // lum::rdr