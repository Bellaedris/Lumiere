//
// Created by Bellaedris on 14/01/2026.
//

#pragma once

#include "IMaterial.h"
#include <Lumiere/GPU/Texture.h>
#include <imgui/imgui.h>

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

    float m_emissionStrength {1.f};
    #pragma endregion Members
public:
    MaterialPBR
    (
        const gpu::TexturePtr &albedo,
        const gpu::TexturePtr &normal,
        const gpu::TexturePtr &metalRough,
        const gpu::TexturePtr &emissive
    );
    void Bind(const gpu::ShaderPtr& shader) override;
    void DrawEditor() override;
};
} // lum::gfx