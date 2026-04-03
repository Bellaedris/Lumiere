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
    gpu::TexturePtr m_albedoTexture{nullptr};
    glm::vec3 m_albedoColor {1.f, 1.f, 1.f};

    gpu::TexturePtr m_normalTexture{nullptr};

    gpu::TexturePtr m_metalRoughTexture{nullptr};

    gpu::TexturePtr m_emissiveTexture{nullptr};
    bool m_useEmission {false};
    float m_emissionStrength {1.f};
    glm::vec3 m_emissionColor {1., 1., 1.};
    #pragma endregion Members
public:
    MaterialPBR() = default;
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