//
// Created by Bellaedris on 29/01/2026.
//

#pragma once
#include "Lumiere/Renderer/IPass.h"

namespace lum::rdr
{
class ShadePBR : public IPass
{
private:
    std::unique_ptr<gpu::Framebuffer> m_framebuffer {nullptr};

    bool m_accumulate {false};
public:
    #pragma region Constants
    constexpr static const char* SHADE_PBR_NAME = "SHADE_PBR";

    constexpr static const char* SHADE_PBR_SHADER_NAME = "SHADE_PBR_SHADER";
    #pragma endregion Constants

    ShadePBR(uint32_t width, uint32_t height);
    void Render(const FrameData &frameData) override;
    void RenderUI() override;

    void Rebuild(uint32_t width, uint32_t height) override;
};
} // lum::rdr