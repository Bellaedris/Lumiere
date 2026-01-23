//
// Created by Bellaedris on 23/01/2026.
//

#pragma once
#include "Lumiere/Renderer/IPass.h"

namespace lum::rdr
{
class Outline : public IPass
{
private:
    uint32_t m_width;
    uint32_t m_height;
public:
    #pragma region Constants
    constexpr static const char* OUTLINE_SOBEL_SHADER_NAME = "OUTLINE_SOBEL_SHADER";

    constexpr static const char* OUTLINE_SOBEL_NAME = "OUTLINE_SOBEL";
    #pragma endregion Constants

    Outline(uint32_t width, uint32_t height);

    void Render(const SceneDesc &scene) override;
};
} // lum::rdr