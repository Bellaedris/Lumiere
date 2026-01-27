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

public:
    virtual ~IPass() = default;

    virtual void Render(const SceneDesc &scene) = 0;
    virtual void Rebuild(uint32_t width, uint32_t height) = 0;
};
} // lum::rdr