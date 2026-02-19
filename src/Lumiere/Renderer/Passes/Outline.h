//
// Created by Bellaedris on 23/01/2026.
//

#pragma once
#include "Lumiere/Renderer/IPass.h"

namespace lum::rdr
{
class Outline : public IPass
{
private:
    float m_lineWidth {1.f};

    uint32_t m_width;
    uint32_t m_height;
public:
    static bool m_registered;
    #pragma region Constants
    constexpr static const char* OUTLINE_SOBEL_SHADER_NAME = "OUTLINE_SOBEL_SHADER";

    constexpr static const char* OUTLINE_SOBEL_NAME = "OUTLINE_SOBEL";
    #pragma endregion Constants

    Outline() = default;
    Outline(uint32_t width, uint32_t height);

    void Init() override;
    void Render(const FrameData &frameData) override;
    void RenderUI() override;

    void Rebuild(uint32_t width, uint32_t height) override;

    void Serialize(YAML::Node passes) override;
    void Deserialize(YAML::Node pass) override;
};
} // lum::rdr