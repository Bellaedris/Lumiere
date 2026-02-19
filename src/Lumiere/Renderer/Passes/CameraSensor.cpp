//
// Created by Bellaedris on 12/02/2026.
//

#include "CameraSensor.h"

#include "ChromaticAberration.h"
#include "ShadePBR.h"
#include "Lumiere/ResourcesManager.h"

namespace lum::rdr
{

REGISTER_TO_PASS_FACTORY(CameraSensor, CameraSensor::CAMERA_SENSOR_NAME);

CameraSensor::CameraSensor(uint32_t width, uint32_t height)
    : m_width(width)
    , m_height(height)
{
    CameraSensor::Init();
}

void CameraSensor::Init()
{
    std::vector<gpu::Shader::ShaderSource> shaderSources =
    {
        {gpu::Shader::ShaderType::Compute, "shaders/cameraSensor.comp"}
    };
    ResourcesManager::Instance()->CacheShader(CAMERA_SENSOR_SHADER_NAME, shaderSources);

    gpu::Texture::TextureDesc output
    {
        .target = gpu::Texture::Target2D,
        .width = static_cast<int>(m_width),
        .height = static_cast<int>(m_height),
        .format = gpu::Texture::RGBA,
        .dataType = gpu::GLUtils::Float,
        .minFilter = gpu::Texture::Nearest,
        .magFilter = gpu::Texture::Nearest,
        .wrapMode = gpu::Texture::WrapMode::ClampToEdge
    };
    ResourcesManager::Instance()->CreateTexture(CAMERA_SENSOR_NAME, output);
}

void CameraSensor::Render(const FrameData &frameData)
{
    if (frameData.profilerGPU)
        frameData.profilerGPU->BeginScope("Camera Sensor");

    gpu::ShaderPtr colorShader = ResourcesManager::Instance()->GetShader(CAMERA_SENSOR_SHADER_NAME);
    colorShader->Bind();
    colorShader->UniformData("iso", m_iso);

    gpu::TexturePtr in = ResourcesManager::Instance()->GetTexture(ShadePBR::SHADE_PBR_NAME);
    in->Bind(0);

    gpu::TexturePtr output = ResourcesManager::Instance()->GetTexture(CAMERA_SENSOR_NAME);
    output->BindImage(1, 0, gpu::GLUtils::Write);

    colorShader->Dispatch(std::ceil(m_width / 16) + 1, std::ceil(m_height / 16) + 1, 1);
    colorShader->Wait();

    if (frameData.profilerGPU)
        frameData.profilerGPU->EndScope("Camera Sensor");
}

void CameraSensor::RenderUI()
{
    if (ImGui::TreeNode("Camera sensor"))
    {
        ImGui::SliderFloat("ISO value", &m_iso, 0.0f, 10.f);
        ImGui::TreePop();
    }
}

void CameraSensor::Rebuild(uint32_t width, uint32_t height)
{
    m_width = width;
    m_height = height;

    gpu::TexturePtr output = ResourcesManager::Instance()->GetTexture(CAMERA_SENSOR_NAME);
    output->SetSize(static_cast<int>(width), static_cast<int>(height));
    output->Reallocate();
}

void CameraSensor::Serialize(YAML::Node passes)
{
    YAML::Node cameraSensor;
    cameraSensor["name"] = CAMERA_SENSOR_NAME;
    cameraSensor["width"] = m_width;
    cameraSensor["height"] = m_height;
    cameraSensor["iso"] = m_iso;
    passes.push_back(cameraSensor);
}

void CameraSensor::Deserialize(YAML::Node pass)
{
    m_width = pass["width"].as<uint32_t>();
    m_height = pass["height"].as<uint32_t>();
    m_iso = pass["iso"].as<float>();
    Init();
}
}
