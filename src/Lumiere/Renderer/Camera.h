//
// Created by Bellaedris on 11/12/2025.
//

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../VectorUtils.h"

namespace lum::rdr
{
class Camera
{
public:
    enum CameraDirection
    {
        Forward,
        Backward,
        Left,
        Right,
        Up,
        Down
    };
private:
    glm::vec3 m_position;
    glm::vec3 m_forward;
    glm::vec3 m_up;
    glm::vec3 m_right;
    float m_fov;
    float m_aspectRatio;
    float m_znear;
    float m_zfar;
    float m_pitch {0};
    float m_yaw {0};
    float m_speed {5.f};
    float m_viewSpeed {.1f};

    glm::mat4 m_view;
    glm::mat4 m_proj;

    bool m_updateView {true};
    bool m_updateProj {true};

    static constexpr float PITCH_LIMIT = 89.f;

    void UpdateView();
    void UpdateProjection();

    void UpdateRotation();
public:
    Camera(const glm::vec3& pos, float aspectRatio, float fov, float znear, float zfar);

    const glm::mat4& Projection();
    const glm::mat4& View();
    const glm::vec3& Position() const;

    void ProcessMouseMovement(const glm::vec2& offset);
    void UpdatePosition(glm::vec3 dir, float dt);

    void SetAspect(float aspect);

    float ZNear() const { return m_znear; };
    float ZFar() const { return m_zfar; };
};
}