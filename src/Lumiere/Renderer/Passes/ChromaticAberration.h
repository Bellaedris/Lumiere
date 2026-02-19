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
    float m_exponent {4};
public:
    static bool m_registered;
    #pragma region Constants
    constexpr static const char* CHROMATIC_ABERRATION_SHADER_NAME = "CHROMATIC_ABERRATION_SHADER";

    constexpr static const char* CHROMATIC_ABERRATION_NAME = "CHROMATIC_ABERRATION";
    #pragma endregion Constants

    ChromaticAberration() = default;
    ChromaticAberration(uint32_t width, uint32_t height);

    void Init() override;

    void Render(const FrameData &frameData) override;

    void RenderUI() override;

    void Rebuild(uint32_t width, uint32_t height) override;

    void Serialize(YAML::Node passes) override;
    void Deserialize(YAML::Node pass) override;
};
}
