//
// Created by Bellaedris on 10/02/2026.
//

#pragma once
#include "Lumiere/Renderer/IPass.h"

namespace lum::rdr
{
class DepthOfField : public IPass
{
private:
    uint32_t m_width;
    uint32_t m_height;

    uint32_t m_intermediateWidth;
    uint32_t m_intermediateHeight;

    float m_focusDistance = 50;
    float m_focusRange = 10;
public:
    #pragma region Constants
    constexpr static const char* DOF_SEPARATION_NAME = "DOF_SEPARATION";
    constexpr static const char* DOF_TEMP_BLUR_NAME = "DOF_TEMP_BLUR";
    constexpr static const char* DOF_NEAR_BLUR_NAME = "DOF_NEAR_BLUR";
    constexpr static const char* DOF_FAR_BLUR_NAME = "DOF_FAR_BLUR";
    constexpr static const char* DOF_NAME = "DOF";

    constexpr static const char* DOF_PLANE_SEPARATION_SHADER_NAME = "DOF_PLANE_SEPARATION_SHADER";
    constexpr static const char* DOF_NEAR_BLUR_SHADER_NAME = "DOF_NEAR_BLUR_SHADER";
    constexpr static const char* DOF_FAR_BLUR_SHADER_NAME = "DOF_FAR_BLUR_SHADER";
    constexpr static const char* DOF_COMPOSITE_SHADER_NAME = "DOF_COMPOSITE_SHADER";
    #pragma endregion Constants

    DepthOfField(uint32_t width, uint32_t height);

    void Render(const SceneDesc &scene) override;

    void RenderUI() override;

    void Rebuild(uint32_t width, uint32_t height) override;
};
} // lum::rdr