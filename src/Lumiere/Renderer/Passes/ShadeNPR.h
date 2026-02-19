//
// Created by Bellaedris on 22/01/2026.
//

#pragma once
#include "Lumiere/Renderer/IPass.h"

namespace lum::rdr
{
class ShadeNPR : public IPass
{
private:
    uint32_t m_width;
    uint32_t m_height;
    std::unique_ptr<gpu::Framebuffer> m_framebuffer {nullptr};
public:
    static bool m_registered;
    #pragma region Constants
    constexpr static const char* SHADE_NPR_NAME = "SHADE_NPR";

    constexpr static const char* SHADE_NPR_SHADER_NAME = "SHADE_NPR_SHADER";
    constexpr static const char* SHADE_NPR_PENCIL_SHADOW_TEXTURE_PATH = "resources/Textures/Draw_Tileable.png";
    #pragma endregion Constants

    ShadeNPR() = default;
    ShadeNPR(uint32_t width, uint32_t height);

    void Init() override;
    void Render(const FrameData &frameData) override;
    void RenderUI() override;

    void Rebuild(uint32_t width, uint32_t height) override;

    void Serialize(YAML::Node passes) override;
    void Deserialize(YAML::Node pass) override;
};
} // lum::rdr