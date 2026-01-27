//
// Created by Bellaedris on 11/12/2025.
//

#include "Camera.h"

namespace lum::rdr
{
    Camera::Camera(const glm::vec3 &pos, float aspectRatio, float fov, float znear, float zfar)
        : m_position(pos)
        , m_aspectRatio(aspectRatio)
        , m_fov(glm::radians(fov))
        , m_znear(znear)
        , m_zfar(zfar)
        , m_forward({0, 0, 1})
        , m_up({0, 1, 0})
    {
        UpdateRotation();
        UpdateView();
        UpdateProjection();
    }

    void Camera::UpdateView()
    {
        m_view = glm::lookAt(m_position, m_position + m_forward, m_up);
        m_updateView = false;
    }

    void Camera::UpdateProjection()
    {
        m_proj = glm::perspective(m_fov, m_aspectRatio, m_znear, m_zfar);
        m_updateProj = false;
    }

    const glm::mat4 &Camera::Projection()
    {
        if(m_updateProj)
            UpdateProjection();
        return m_proj;
    }

    const glm::mat4 &Camera::View()
    {
        if(m_updateView)
            UpdateView();
        return m_view;
    }

    void Camera::UpdatePosition(glm::vec3 dir, float dt)
    {
        float vel = dt * m_speed;
        // change dir from camera space to world space
        dir = glm::mat3(m_right, m_up, m_forward) * dir;
        m_position += dir * vel;

        m_updateView = true;
    }

    void Camera::SetAspect(float aspect)
    {
        m_aspectRatio = aspect;
        UpdateProjection();
    }

    void Camera::UpdateRotation()
    {
        float yawRad = glm::radians(m_yaw);
        float pitchRad = glm::radians(m_pitch);
        glm::vec3 newDir
        {
            std::cos(yawRad) * std::cos(pitchRad),
            std::sin(pitchRad),
            std::sin(yawRad) * std::cos(pitchRad)
        };

        m_forward = glm::normalize(newDir);
        m_right = glm::normalize(glm::cross(m_forward, VectorUtils::UP));
        m_up = glm::normalize(glm::cross(m_right, m_forward));

        m_updateView = true;
    }

    void Camera::ProcessMouseMovement(const glm::vec2& offset)
    {
        // avoid small movements
        float offsetX = std::abs(offset.x) <= 1.f ? 0.f : offset.x;
        float offsetY = std::abs(offset.y) <= 1.f ? 0.f : offset.y;

        m_yaw += offsetX * m_viewSpeed;
        m_pitch += offsetY * m_viewSpeed;

        if (m_pitch > PITCH_LIMIT)
            m_pitch = PITCH_LIMIT;
        if (m_pitch < -PITCH_LIMIT)
            m_pitch = -PITCH_LIMIT;

        UpdateRotation();
    }
}