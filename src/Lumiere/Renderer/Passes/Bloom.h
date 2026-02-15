//
// Created by Bellaedris on 03/02/2026.
//

#pragma once
#include "Lumiere/Renderer/IPass.h"

namespace lum::rdr
{
class Bloom : public IPass
{
    uint32_t m_width;
    uint32_t m_height;

    float m_intensity { .0f };
public:
    #pragma region Constants
    constexpr static int MIP_CHAIN_SIZE = 6;
    constexpr static const char* BLOOM_MASK_NAME = "BLOOM_MASK";
    constexpr static const char* BLOOM_NAME = "BLOOM";
    constexpr static const char* BLOOM_DOWNSAMPLE_SHADER_NAME = "BLOOM_DOWNSAMPLE_SHADER";
    constexpr static const char* BLOOM_UPSAMPLE_SHADER_NAME = "BLOOM_UPSAMPLE_SHADER";
    constexpr static const char* BLOOM_COMPOSITE_SHADER_NAME = "BLOOM_COMPOSITE_SHADER";
    #pragma endregion Constants

    Bloom(uint32_t width, uint32_t height);

    void Render(const FrameData &frameData) override;

    void RenderUI() override;

    void Rebuild(uint32_t width, uint32_t height) override;
};
}
