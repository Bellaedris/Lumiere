//
// Created by Bellaedris on 14/01/2026.
//

#include "MaterialPBR.h"

#include "Lumiere/ResourcesManager.h"
#include "Lumiere/Utils/FileUtils.h"

namespace lum::gfx
{
    MaterialPBR::MaterialPBR
    (
        const gpu::TexturePtr &albedo,
        const gpu::TexturePtr &normal,
        const gpu::TexturePtr &metalRough,
        const gpu::TexturePtr &emissive
    )
        : m_albedoTexture(albedo)
        , m_normalTexture(normal)
        , m_metalRoughTexture(metalRough)
        , m_emissiveTexture(emissive)
    {}

    void MaterialPBR::Bind(const gpu::ShaderPtr& shader)
    {
        gpu::TexturePtr albedo = m_albedoTexture == nullptr
                                       ? ResourcesManager::Instance()->GetTexture
                                       (ResourcesManager::DEFAULT_TEXTURE_WHITE_NAME)
                                       : m_albedoTexture;
        albedo->Bind(0);
        shader->UniformData("AlbedoTexture", 0);

        if (m_normalTexture != nullptr)
        {
            m_normalTexture->Bind(1);
            shader->UniformData("NormalsTexture", 1);
            shader->UniformData("hasNormals", 0);
        }
        else
        {
            shader->UniformData("hasNormals", 1);
        }

        gpu::TexturePtr metalRough = m_metalRoughTexture == nullptr
                                       ? ResourcesManager::Instance()->GetTexture
                                       (ResourcesManager::DEFAULT_TEXTURE_BLACK_NAME)
                                       : m_metalRoughTexture;
        metalRough->Bind(2);
        shader->UniformData("MetalRoughTexture", 2);

        gpu::TexturePtr emissive = m_emissiveTexture == nullptr
                                       ? ResourcesManager::Instance()->GetTexture
                                       (ResourcesManager::DEFAULT_TEXTURE_BLACK_NAME)
                                       : m_emissiveTexture;
        emissive->Bind(3);
        shader->UniformData("EmissiveTexture", 3);

        shader->UniformData("emissionStrength", m_emissionStrength);
    }

    void MaterialPBR::DrawEditor()
    {
        ImGui::DragFloat("Emission Strength", &m_emissionStrength, 1.f);
    }
} // lum::gfx