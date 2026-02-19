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

    float m_radius    {.5f};
    float m_tightness {1.f};

public:
    static bool m_registered;
    #pragma region Constants
    constexpr static const char* VIGNETTE_SHADER_NAME = "VIGNETTE_SHADER";

    constexpr static const char* VIGNETTE_NAME = "VIGNETTE_MASK";
    #pragma endregion Constants

    Vignette() = default;
    Vignette(uint32_t width, uint32_t height);

    void Init() override;
    void Render(const FrameData &frameData) override;
    void RenderUI() override;

    void Rebuild(uint32_t width, uint32_t height) override;

    void Serialize(YAML::Node passes) override;
    void Deserialize(YAML::Node pass) override;
};
}