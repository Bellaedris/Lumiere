//
// Created by Bellaedris on 12/02/2026.
//

#pragma once
#include "Lumiere/Renderer/IPass.h"

namespace lum::rdr
{
class CameraSensor : public IPass
{
protected:
    uint32_t m_width;
    uint32_t m_height;

    float m_iso {1.f};
public:
    static bool m_registered;
    #pragma region Constants
    constexpr static const char* CAMERA_SENSOR_SHADER_NAME = "CAMERA_SENSOR_SHADER";

    constexpr static const char* CAMERA_SENSOR_NAME = "CAMERA_SENSOR";
    #pragma endregion Constants

    CameraSensor() = default;
    CameraSensor(uint32_t width, uint32_t height);

    void Init() override;

    void Render(const FrameData &frameData) override;

    void RenderUI() override;

    void Rebuild(uint32_t width, uint32_t height) override;

    void Serialize(YAML::Node passes) override;

    void Deserialize(YAML::Node pass) override;
};
}
