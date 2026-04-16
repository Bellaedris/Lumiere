//
// Created by Bellaedris on 15/04/2026.
//

#pragma once
#include "Lumiere/Renderer/IPass.h"

namespace lum::rdr
{
class UI : public IPass
{
protected:
    uint32_t m_width;
    uint32_t m_height;

    std::unique_ptr<gpu::Framebuffer> m_framebuffer {nullptr};
public:
    static bool m_registered;
    #pragma region Constants
    constexpr static const char* UI_SHADER_NAME = "UI_SHADER";

    constexpr static const char* UI_NAME = "UI";
    #pragma endregion Constants

    UI() = default;
    UI(uint32_t width, uint32_t height);

    void Init() override;

    void Render(const FrameData &frameData) override;

    void RenderUI() override;

    void Rebuild(uint32_t width, uint32_t height) override;

    void Serialize(YAML::Node passes) override;

    void Deserialize(YAML::Node pass) override;
};
}
