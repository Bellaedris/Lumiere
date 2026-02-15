//
// Created by Bellaedris on 22/01/2026.
//

#include "Lights.h"

#include <chrono>

namespace lum::rdr {
LightList::LightList()
    : m_pointLightsBuffer(std::make_unique<gpu::Buffer>(gpu::Buffer::BufferType::Uniform))
    , m_dirLightBuffer(std::make_unique<gpu::Buffer>(gpu::Buffer::BufferType::Uniform))
    , m_spotlightBuffer(std::make_unique<gpu::Buffer>(gpu::Buffer::BufferType::Uniform))
{

}

void LightList::AddPointLight(glm::vec3 &position, float intensity, glm::vec3 &color, float radius)
{
    m_pointLights.push_back({position, intensity, color, radius});
}

void LightList::AddDirLight(const glm::vec3 &direction, float intensity, const glm::vec3 &color)
{
    m_directionalLights.push_back({direction, intensity, color, 0});
}

void LightList::Update()
{
    // float t = std::chrono::duration<float>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    // m_directionalLights[0].m_direction.x = std::sin(t);
    // m_directionalLights[0].m_direction.z = std::cosf(t);

    m_pointLightsBuffer->Write(sizeof(PointLight) * m_pointLights.size(), m_pointLights.data(), gpu::Buffer::DynamicDraw);
    m_dirLightBuffer->Write(sizeof(DirectionalLight) * m_directionalLights.size(), m_directionalLights.data(), gpu::Buffer::DynamicDraw);
    m_spotlightBuffer->Write(sizeof(SpotLight) * m_spotLights.size(), m_spotLights.data(), gpu::Buffer::DynamicDraw);
}

void LightList::Bind(int pointLightBind, int dirLightBind, int spotLightBind) const
{
    if (m_directionalLights.empty() == false)
        m_dirLightBuffer->Bind(dirLightBind);

    if (m_pointLights.empty() == false)
        m_pointLightsBuffer->Bind(pointLightBind);

    if (m_spotLights.empty() == false)
        m_spotlightBuffer->Bind(spotLightBind);
}
} // lum::rdr