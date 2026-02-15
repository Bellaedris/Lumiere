//
// Created by Bellaedris on 06/02/2026.
//

#pragma once
#include "Lumiere/Renderer/IPass.h"

namespace lum::rdr
{
class ChromaticAberration : public IPass
{
protected:
    uint32_t m_width;
    uint32_t m_height;

    int m_samples {3};
    float m_intensity {.5f};
    int m_exponent {4};
public:
    #pragma region Constants
    constexpr static const char* CHROMATIC_ABERRATION_SHADER_NAME = "CHROMATIC_ABERRATION_SHADER";

    constexpr static const char* CHROMATIC_ABERRATION_NAME = "CHROMATIC_ABERRATION";
    #pragma endregion Constants

    ChromaticAberration(uint32_t width, uint32_t height);
    void Render(const FrameData &frameData) override;

    void RenderUI() override;

    void Rebuild(uint32_t width, uint32_t height) override;
};
}
