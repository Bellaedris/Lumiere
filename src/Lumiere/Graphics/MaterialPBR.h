//
// Created by Bellaedris on 14/01/2026.
//

#pragma once

#include "IMaterial.h"
#include <Lumiere/GPU/Texture.h>
#include <yaml-cpp/yaml.h>
#include <Lumiere/Utils/YAMLUtils.h>
#include <imgui/imgui.h>

#include "stduuid/uuid.h"

namespace lum::gfx
{
class MaterialPBR : public IMaterial
{
public:
    #pragma region Constants
    constexpr static int SERIALIZER_VERSION = 1;
    constexpr static const char* SERIALIZER_NAME = "PBR Lit";
    #pragma endregion // Constants
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

    static bool m_registered;
    #pragma endregion Members
public:
    MaterialPBR(const std::string& name);
    MaterialPBR
    (
        const std::string& name,
        const std::string &albedoPath,
        const std::string &normalPath,
        const std::string &metalRoughPath,
        const std::string &emissivePath
    );
    void Bind(const gpu::ShaderPtr& shader) override;
    void DrawEditor() override;

    void Serialize(const std::string &path) const override;
    void Deserialize(YAML::Node &node) override;
};
} // lum::gfx