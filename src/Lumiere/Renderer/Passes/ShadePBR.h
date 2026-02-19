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
    uint32_t m_width{};
    uint32_t m_height;
    std::unique_ptr<gpu::Framebuffer> m_framebuffer {nullptr};

    bool m_accumulate {false};
public:
    static bool m_registered;
    #pragma region Constants
    constexpr static const char* SHADE_PBR_NAME = "SHADE_PBR";

    constexpr static const char* SHADE_PBR_SHADER_NAME = "SHADE_PBR_SHADER";
    #pragma endregion Constants

    ShadePBR() = default;
    ShadePBR(uint32_t width, uint32_t height);

    void Init() override;
    void Render(const FrameData &frameData) override;
    void RenderUI() override;

    void Rebuild(uint32_t width, uint32_t height) override;

    void Serialize(YAML::Node passes) override;
    void Deserialize(YAML::Node pass) override;
};
} // lum::rdr