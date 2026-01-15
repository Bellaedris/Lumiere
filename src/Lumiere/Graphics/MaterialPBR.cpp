//
// Created by Bellaedris on 14/01/2026.
//

#include "MaterialPBR.h"

namespace lum::gfx
{
    MaterialPBR::MaterialPBR
    (
        const gpu::ShaderPtr &shader,
        const gpu::TexturePtr &albedo,
        const gpu::TexturePtr &normal,
        const gpu::TexturePtr &metalRough,
        const gpu::TexturePtr &emissive
    )
        : IMaterial(shader)
        , m_albedoTexture(albedo)
        , m_normalTexture(normal)
        , m_metalRoughTexture(metalRough)
        , m_emissiveTexture(emissive)
    {}

    void MaterialPBR::Bind()
    {
        m_shader->Bind();
        m_albedoTexture->Bind(0);
        m_shader->UniformData("AlbedoTexture", 0);
    }
} // lum::gfx