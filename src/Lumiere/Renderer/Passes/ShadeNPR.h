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
    std::unique_ptr<gpu::Framebuffer> m_framebuffer {nullptr};
public:
    #pragma region Constants
    constexpr static const char* SHADE_NPR_NAME = "SHADE_NPR";

    constexpr static const char* SHADE_NPR_SHADER_NAME = "SHADE_NPR_SHADER";
    constexpr static const char* SHADE_NPR_PENCIL_SHADOW_TEXTURE_PATH = "resources/Textures/Draw_Tileable.png";
    #pragma endregion Constants

    ShadeNPR(uint32_t width, uint32_t height);
    void Render(const FrameData &frameData) override;
    void RenderUI() override;

    void Rebuild(uint32_t width, uint32_t height) override;
};
} // lum::rdr