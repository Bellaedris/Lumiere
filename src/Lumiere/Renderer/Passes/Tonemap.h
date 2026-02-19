//
// Created by Bellaedris on 30/01/2026.
//

#pragma once
#include "Lumiere/Renderer/IPass.h"

namespace lum::rdr
{
class Tonemap : public IPass
{
private:
    int m_technique {1};
    float m_gamma {2.2f};

    const std::array<const char*, 3> m_techniques = {"Reinhard", "ACES", "Uncharted 2 Filmic"};

    uint32_t m_width;
    uint32_t m_height;
public:
    static bool m_registered;
    #pragma region Constants
    constexpr static const char* TONEMAP_SHADER_NAME = "TONEMAP_SHADER";

    constexpr static const char* TONEMAP_NAME = "TONEMAP";
    #pragma endregion Constants

    Tonemap() = default;
    Tonemap(uint32_t width, uint32_t height);

    void Init() override;
    void Render(const FrameData &frameData) override;
    void RenderUI() override;

    void Rebuild(uint32_t width, uint32_t height) override;

    void Serialize(YAML::Node passes) override;
    void Deserialize(YAML::Node pass) override;
};
} // lum::rdr