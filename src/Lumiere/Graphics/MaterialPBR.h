//
// Created by Bellaedris on 14/01/2026.
//

#pragma once

#include "IMaterial.h"
#include <Lumiere/GPU/Texture.h>
#include <array>

namespace lum::gfx
{
class MaterialPBR : public IMaterial
{
public:
private:
    #pragma region Members
    gpu::TexturePtr m_albedoTexture;
    gpu::TexturePtr m_normalTexture;
    gpu::TexturePtr m_metalRoughTexture;
    gpu::TexturePtr m_emissiveTexture;
    #pragma endregion Members
public:
    MaterialPBR
    (
        const gpu::ShaderPtr & shader,
        const gpu::TexturePtr &albedo,
        const gpu::TexturePtr &normal,
        const gpu::TexturePtr &metalRough,
        const gpu::TexturePtr &emissive
    );
    void Bind() override;
};
} // lum::gfx