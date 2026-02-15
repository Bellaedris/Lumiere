//
// Created by Bellaedris on 06/02/2026.
//

#pragma once
#include "Lumiere/Renderer/IPass.h"

namespace lum::rdr
{
class Vignette : public IPass
{
protected:
    uint32_t m_width;
    uint32_t m_height;

    float m_radius {.5f};
public:
    #pragma region Constants
    constexpr static const char* VIGNETTE_SHADER_NAME = "VIGNETTE_SHADER";

    constexpr static const char* VIGNETTE_NAME = "VIGNETTE_MASK";
    #pragma endregion Constants

    Vignette(uint32_t width, uint32_t height);
    void Render(const FrameData &frameData) override;

    void RenderUI() override;

    void Rebuild(uint32_t width, uint32_t height) override;
};
}