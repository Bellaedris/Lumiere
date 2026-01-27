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
    std::unique_ptr<gpu::Framebuffer> m_framebuffer {nullptr};
public:
    #pragma region Constants
    constexpr static const char* COMPOSITE_NPR_SHADER_NAME = "COMPOSITE_NPR_SHADER";
    #pragma endregion Constants

    CompositeNPR(uint32_t width, uint32_t height);
    void Render(const SceneDesc &scene) override;

    void Rebuild(uint32_t width, uint32_t height) override;
};
} // lum::rdr