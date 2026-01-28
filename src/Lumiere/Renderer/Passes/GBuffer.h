//
// Created by Bellaedris on 20/01/2026.
//

#pragma once
#include "../IPass.h"

namespace lum::rdr
{
/**
 * \brief The GBuffer will draw geometry into color attachments for albedo, positions, normal and depth
 */
class GBuffer : public IPass
{
private:
    std::unique_ptr<gpu::Framebuffer> m_framebuffer {nullptr};
public:
    #pragma region constants
    constexpr static const char* GBUFFER_ALBEDO_NAME = "GBUFFER_ALBEDO";
    constexpr static const char* GBUFFER_NORMALS_NAME = "GBUFFER_NORMALS";
    constexpr static const char* GBUFFER_POSITIONS_NAME = "GBUFFER_POSITIONS";
    constexpr static const char* GBUFFER_DEPTH_NAME = "GBUFFER_DEPTH";

    constexpr static const char* GBUFFER_SHADER_NAME = "GBUFFER_SHADER";
    #pragma endregion constants

    GBuffer(uint32_t width, uint32_t height);
    void Render(const SceneDesc &scene) override;
    void RenderUI() override;

    void Rebuild(uint32_t width, uint32_t height) override;
};
} // lum::rdr