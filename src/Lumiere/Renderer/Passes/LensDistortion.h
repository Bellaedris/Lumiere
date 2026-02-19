//
// Created by Bellaedris on 06/02/2026.
//

#pragma once
#include "Lumiere/Renderer/IPass.h"

namespace lum::rdr
{
/**
 * \brief Heavily inspired by Unity's work here
 * https://github.com/Unity-Technologies/PostProcessing/blob/v2/PostProcessing/Runtime/Effects/LensDistortion.cs
 * and here
 * https://github.com/Unity-Technologies/PostProcessing/blob/v2/PostProcessing/Shaders/Builtins/Distortion.hlsl
 */
class LensDistortion : public IPass
{
protected:
    uint32_t m_width;
    uint32_t m_height;

    float m_intensityX {1.f};
    float m_intensityY {1.f};
    float m_centerX {.0f};
    float m_centerY {.0f};
    float m_distortionIntensity {1.f};
    float m_screenScale {1.f};
public:
    static bool m_registered;
    #pragma region Constants
    constexpr static const char* LENS_DISTORTION_SHADER_NAME = "LENS_DISTORTION_SHADER";

    constexpr static const char* LENS_DISTORTION_NAME = "LENS_DISTORTION";
    #pragma endregion Constants

    LensDistortion() = default;
    LensDistortion(uint32_t width, uint32_t height);

    void Init() override;

    void Render(const FrameData &frameData) override;

    void RenderUI() override;

    void Rebuild(uint32_t width, uint32_t height) override;

    void Serialize(YAML::Node passes) override;
    void Deserialize(YAML::Node pass) override;
};
}
