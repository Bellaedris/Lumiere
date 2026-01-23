//
// Created by Bellaedris on 22/01/2026.
//

#pragma once
#include "Lumiere/Renderer/IPass.h"

namespace lum::rdr
{
class ShadeNPR : public IPass
{
public:
    #pragma region Constants
    constexpr static const char* SHADE_NPR_SHADER_NAME = "SHADE_NPR_SHADER";
    constexpr static const char* SHADE_NPR_PENCIL_SHADOW_TEXTURE_PATH = "resources/Textures/Draw_Tileable.png";
    #pragma endregion Constants

    ShadeNPR();
    void Render(const SceneDesc &scene) override;
};
} // lum::rdr