//
// Created by Bellaedris on 14/01/2026.
//

#include "MaterialPBR.h"

#include <fstream>

#include "Lumiere/ResourcesManager.h"
#include "Lumiere/Utils/FileUtils.h"

namespace lum::gfx
{
    REGISTER_TO_MATERIAL_FACTORY(MaterialPBR, MaterialPBR::SERIALIZER_NAME);

    MaterialPBR::MaterialPBR
    (
        const std::string& name,
        const std::string &albedoPath,
        const std::string &normalPath,
        const std::string &metalRoughPath,
        const std::string &emissivePath
    )
        : IMaterial(name)
    {
        std::function<gpu::TexturePtr(const std::string&)> LoadTextureFromCache = [&](const std::string& path)
        {
            gpu::TexturePtr t = ResourcesManager::Instance()->GetTexture(path);
            if (t == nullptr && path.empty() == false)
                t = ResourcesManager::Instance()->CacheTexture(gpu::Texture::TextureTarget::Target2D, path, true);
            return t;
        };
        m_albedoTexture = LoadTextureFromCache(albedoPath);
        m_normalTexture = LoadTextureFromCache(normalPath);
        m_metalRoughTexture = LoadTextureFromCache(metalRoughPath);
        m_emissiveTexture = LoadTextureFromCache(emissivePath);
    }

    MaterialPBR::MaterialPBR(const std::string& name) : IMaterial(name) {}

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
        if (ImGui::ColorEdit3("Albedo color", glm::value_ptr(m_albedoColor)))
            Serialize(ResourcesManager::MATERIAL_SAVE_PATH);

        ImGui::Checkbox("Override emission", &m_useEmission);
        if (m_emissiveTexture == nullptr && m_useEmission == false)
            ImGui::BeginDisabled();

        if (ImGui::ColorEdit3("Emission color", glm::value_ptr(m_emissionColor)))
            Serialize(ResourcesManager::MATERIAL_SAVE_PATH);
        if (ImGui::DragFloat("Emission Strength", &m_emissionStrength, 1.f))
            Serialize(ResourcesManager::MATERIAL_SAVE_PATH);

        if (m_emissiveTexture == nullptr && m_useEmission == false)
            ImGui::EndDisabled();
    }

    void MaterialPBR::Serialize(const std::string &path) const
    {
        YAML::Node material;
        material["version"] = SERIALIZER_VERSION;
        material["type"] = SERIALIZER_NAME;
        material["albedo"] = m_albedoTexture == nullptr ? "" : m_albedoTexture->Path();
        material["normal"] = m_normalTexture == nullptr ? "" : m_normalTexture->Path();
        material["metalRough"] = m_metalRoughTexture == nullptr ? "" : m_metalRoughTexture->Path();
        material["emissive"] = m_emissiveTexture == nullptr ? "" : m_emissiveTexture->Path();

        material["albedoColor"] = m_albedoColor;

        material["useEmission"] = m_useEmission;
        material["emissiveColor"] = m_emissionColor;
        material["emissiveStrength"] = m_emissionStrength;

        std::string savePath = path + m_name + std::string(MATERIAL_FILE_FORMAT);
        YAML::Emitter emitter;
        emitter << material;
        if (emitter.good() == false)
        {
            std::cerr << "Couldn't serialize the Material " << m_name << ": " << emitter.GetLastError() << "\n";
            return;
        }

        // we could eventually ensure the write process is correct by writing to a tmp file then renaming, but i'm kinda lazy
        std::ofstream out(savePath, std::ios::out | std::ios::trunc);
        out << emitter.c_str();
    }

    void MaterialPBR::Deserialize(YAML::Node &node)
    {
        if (node["version"].as<int>() != SERIALIZER_VERSION)
        {
            std::cerr << "This material uses an older format and cannot be deserialized.\n";
            return;
        }

        ResourcesManager* resources = ResourcesManager::Instance();
        std::function<gpu::TexturePtr(std::string)> GetOrCreate = [&](const std::string& path)
        {
            gpu::TexturePtr tex = resources->GetTexture(path);
            if (tex == nullptr && path.empty() == false)
                tex = resources->CacheTexture(gpu::Texture::Target2D, path, true);

            return tex;
        };

        m_albedoTexture = GetOrCreate(node["albedo"].as<std::string>());
        m_normalTexture = GetOrCreate(node["normal"].as<std::string>());
        m_metalRoughTexture = GetOrCreate(node["metalRough"].as<std::string>());
        m_emissiveTexture = GetOrCreate(node["emissive"].as<std::string>());

        m_albedoColor = node["albedoColor"].as<glm::vec3>();
        m_useEmission = node["useEmission"].as<bool>();
        m_emissionColor = node["emissiveColor"].as<glm::vec3>();
        m_emissionStrength = node["emissiveStrength"].as<float>();
    }
} // lum::gfx