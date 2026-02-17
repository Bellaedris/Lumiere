//
// Created by Bellaedris on 06/02/2026.
//

#pragma once
#include "Lumiere/Renderer/IPass.h"

namespace lum::rdr
{
class LensDistortion : public IPass
{
protected:
    uint32_t m_width;
    uint32_t m_height;

    float m_distortionIntensity {1.f};
    float m_distortionTightness {8.f};
public:
    #pragma region Constants
    constexpr static const char* LENS_DISTORTION_SHADER_NAME = "LENS_DISTORTION_SHADER";

    constexpr static const char* LENS_DISTORTION_NAME = "LENS_DISTORTION";
    #pragma endregion Constants

    LensDistortion(uint32_t width, uint32_t height);
    void Render(const FrameData &frameData) override;

    void RenderUI() override;

    void Rebuild(uint32_t width, uint32_t height) override;
};
}
