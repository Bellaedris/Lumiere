//
// Created by Bellaedris on 22/01/2026.
//

#pragma once
#include "Lumiere/Actor.h"
#include "Lumiere/Actor.h"
#include "Lumiere/GPU/Buffer.h"

namespace lum::rdr
{
class LightList
{
private:
    #pragma region Structs
    // std 140 compliant light structs
    struct PointLight
    {
        glm::vec3 m_position;
        float m_intensity;

        glm::vec3 m_color;
        float m_radius;
    };

    struct SpotLight
    {
        // not supported yet
    };

    struct DirectionalLight
    {
        glm::vec3 m_direction;
        float m_intensity;

        glm::vec3 m_color;
        float m_pad;
    };
    #pragma endregion Structs

    #pragma region Members
    std::unique_ptr<gpu::Buffer> m_pointLightsBuffer;
    std::unique_ptr<gpu::Buffer> m_dirLightBuffer;
    std::unique_ptr<gpu::Buffer> m_spotlightBuffer;

    std::vector<PointLight> m_pointLights;
    std::vector<SpotLight> m_spotLights;
    std::vector<DirectionalLight> m_directionalLights;
    #pragma endregion Members
public:
    LightList();

    /**
     * \brief Add a new point light
     * \param position position of the light
     * \param intensity intensity of the light, a multiplier of the color intensity inside the range
     * \param color color of the light
     * \param radius radius of the light, the range at which there is no more light
     */
    void AddPointLight(glm::vec3& position, float intensity, glm::vec3& color, float radius);

    /**
     * \brief Add a new directional light
     * \param direction direction of the light
     * \param intensity intensity, a multiplier of the color
     * \param color color of the light
     */
    void AddDirLight(const glm::vec3 & direction, float intensity, const glm::vec3 & color);

    /**
     * \brief Updates all Uniform Buffers data with the current storage state
     */
    void Update();

    /**
     * \brief Binds all light buffers to a uniform binding point. If a light type is empty, we skip that type.
     * \param bindingPoint uniform binding point
     */
    void Bind(int pointLightBind, int dirLightBind, int spotLightBind) const;

    // accessors for size
    int PointLightCount() const { return m_pointLights.size(); };
    int DirectionalLightCount() const { return m_directionalLights.size(); };
    int SpotLightCount() const { return m_spotLights.size(); };
};
} // lum::rdr