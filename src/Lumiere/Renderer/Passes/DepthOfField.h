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

    float m_focusDistance = 50;
    float m_focusRange = 10;
public:
    #pragma region Constants
    constexpr static const char* DOF_SEPARATION_NAME = "DOF_SEPARATION";
    constexpr static const char* DOF_NAME = "DOF";

    constexpr static const char* DOF_PLANE_SEPARATION_SHADER_NAME = "DOF_PLANE_SEPARATION_NAME";
    #pragma endregion Constants

    DepthOfField(uint32_t width, uint32_t height);

    void Render(const SceneDesc &scene) override;

    void RenderUI() override;

    void Rebuild(uint32_t width, uint32_t height) override;
};
} // lum::rdr