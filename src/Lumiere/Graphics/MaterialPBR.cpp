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
        shader->UniformData("albedoColor", m_albedoColor);

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

        // emissive is a bit special, if there is no texture and the user does not want any emissive, we default to black
        // otherwise we default to white so the whole object emits and can be controlled from the editor
        gpu::TexturePtr emissive;
        if (m_emissiveTexture == nullptr && m_useEmission == false)
            emissive = ResourcesManager::Instance()->GetTexture(ResourcesManager::DEFAULT_TEXTURE_BLACK_NAME);
        else if (m_emissiveTexture == nullptr && m_useEmission == true)
            emissive = ResourcesManager::Instance()->GetTexture(ResourcesManager::DEFAULT_TEXTURE_WHITE_NAME);
        else
            emissive = m_emissiveTexture;
        emissive->Bind(3);
        shader->UniformData("EmissiveTexture", 3);
        shader->UniformData("emissionColor", m_emissionColor);
        shader->UniformData("emissionStrength", m_emissionStrength);
    }

    void MaterialPBR::DrawEditor()
    {
        ImGui::ColorEdit3("Albedo color", glm::value_ptr(m_albedoColor));

        ImGui::Checkbox("Override emission", &m_useEmission);
        if (m_emissiveTexture == nullptr && m_useEmission == false)
            ImGui::BeginDisabled();

        ImGui::ColorEdit3("Emission color", glm::value_ptr(m_emissionColor));
        ImGui::DragFloat("Emission Strength", &m_emissionStrength, 1.f);

        if (m_emissiveTexture == nullptr && m_useEmission == false)
            ImGui::EndDisabled();
    }
} // lum::gfx