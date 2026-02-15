//
// Created by Bellaedris on 12/02/2026.
//

#pragma once
#include "Lumiere/Renderer/IPass.h"

namespace lum::rdr
{
class ColorAdjustments : public IPass
{
protected:
    uint32_t m_width;
    uint32_t m_height;

    float m_postExposure {.0f};
    float m_contrast {.0f};
    float m_saturation {.0f};
public:
    #pragma region Constants
    constexpr static const char* COLOR_ADJUSTMENTS_SHADER_NAME = "COLOR_ADJUSTMENTS_SHADER";

    constexpr static const char* COLOR_ADJUSTMENTS_NAME = "COLOR_ADJUSTMENTS";
    #pragma endregion Constants

    ColorAdjustments(uint32_t width, uint32_t height);
    void Render(const FrameData &frameData) override;

    void RenderUI() override;

    void Rebuild(uint32_t width, uint32_t height) override;
};
}
