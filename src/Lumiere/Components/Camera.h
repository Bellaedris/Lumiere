//
// Created by belle on 30/03/2026.
//

#pragma once
#include "IComponent.h"

#include <glm/glm.hpp>

namespace lum::comp
{
class Camera : public IComponent
{
private:
    /** \brief projection mode is perspective if true, false is orthographic */
    bool m_perspective {true};
    float m_fov {70.f};
    float m_zNear {0.01f};
    float m_zFar {1000.f};
    float m_orthographicSize {1.f};
    glm::vec2 m_viewportMin {.0f, .0f};
    glm::vec2 m_viewportMax {1.f, 1.f};
    float m_viewportWidth {1.f};
    float m_viewportHeight {1.f};

    static bool m_registered;
    static bool m_typeRegistered;
public:
    Camera(Node3D* node, SystemProvider* systems);

    std::string CameraType() const;
    bool& IsPerspective() { return m_perspective; }
    float& Fov() { return m_fov; }
    float& ZNear() { return m_zNear; }
    float& ZFar() { return m_zFar; }
    float& OrthoSize() { return m_orthographicSize; }
    glm::vec2& ViewportMin() { return m_viewportMin; }
    glm::vec2& ViewportMax() { return m_viewportMax; }

    void SetCameraType(bool value) { m_perspective = value; };

    void Serialize(YAML::Node& node) override;
    void Deserialize(YAML::Node& node) override;
};
} // lum