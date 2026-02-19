//
// Created by Bellaedris on 27/01/2026.
//

#pragma once
#include "Lumiere/Renderer/IPass.h"

namespace lum::rdr
{
class CompositeNPR : public IPass
{
private:
    uint32_t m_width;
    uint32_t m_height;
    std::unique_ptr<gpu::Framebuffer> m_framebuffer {nullptr};
public:
    static bool m_registered;
    #pragma region Constants
    constexpr static const char* COMPOSITE_NPR_NAME = "COMPOSITE_NPR";
    constexpr static const char* COMPOSITE_NPR_SHADER_NAME = "COMPOSITE_NPR_SHADER";
    #pragma endregion Constants

    CompositeNPR() = default;
    CompositeNPR(uint32_t width, uint32_t height);

    void Init() override;
    void Render(const FrameData &frameData) override;
    void RenderUI() override;

    void Rebuild(uint32_t width, uint32_t height) override;

    void Serialize(YAML::Node passes) override;
    void Deserialize(YAML::Node pass) override;
};
} // lum::rdr