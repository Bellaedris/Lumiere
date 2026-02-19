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
    static bool m_registered;
    #pragma region Constants
    constexpr static const char* COLOR_ADJUSTMENTS_SHADER_NAME = "COLOR_ADJUSTMENTS_SHADER";

    constexpr static const char* COLOR_ADJUSTMENTS_NAME = "COLOR_ADJUSTMENTS";
    #pragma endregion Constants

    ColorAdjustments() = default;
    ColorAdjustments(uint32_t width, uint32_t height);

    void Init() override;

    void Render(const FrameData &frameData) override;

    void RenderUI() override;

    void Rebuild(uint32_t width, uint32_t height) override;

    void Serialize(YAML::Node passes) override;
    void Deserialize(YAML::Node pass) override;
};
}
