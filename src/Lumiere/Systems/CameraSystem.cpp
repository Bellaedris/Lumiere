//
// Created by belle on 30/03/2026.
//

#include "CameraSystem.h"

#include "Lumiere/Components/Camera.h"
#include "Lumiere/Events/RenderEvents.h"

namespace lum
{
CameraSystem::CameraSystem(uint32_t width, uint32_t height, const std::shared_ptr<evt::EventHandler> &events)
    : m_viewportWidth(width)
    , m_viewportHeight(height)
    , m_events(events)
{
    LUM_SUB_TO_EVENT(m_events, evt::FramebufferResized, CameraSystem::OnEvent);
}

void CameraSystem::Update(float dt)
{
    switch (m_mode)
    {
        case CameraMode::Editor:
            if (m_editorCamera != nullptr)
                UploadEditorCameraData();
            break;
        case CameraMode::Play:
            if (m_playerCamera == nullptr)
                UploadEditorCameraData();
            else
                UploadGameCameraData();
            break;
    }
}

void CameraSystem::UploadGameCameraData()
{
    comp::Transform* transform = m_playerCamera->GetTransform();
    comp::Camera* camera = m_playerCamera->GetComponent<comp::Camera>().value(); // No check because i know there is a camera here
    m_cameraData.viewMatrix = glm::lookAt(transform->Position(), transform->Position() + transform->Forward(), transform->Up());
    m_cameraData.inverseViewMatrix = glm::inverse(m_cameraData.viewMatrix);
    if (camera->IsPerspective())
        m_cameraData.projectionMatrix = glm::perspective(glm::radians(camera->Fov()), m_viewportWidth / static_cast<float>(m_viewportHeight), camera->ZNear(), camera->ZFar());
    else
    {
        float w = camera->OrthoSize() * m_viewportWidth;
        float h = camera->OrthoSize() * m_viewportHeight;
        m_cameraData.projectionMatrix = glm::ortho(-w, w, -h, h);
    }
    m_cameraData.inverseProjectionMatrix = glm::inverse(m_cameraData.projectionMatrix);
    m_cameraData.position = transform->Position();
    m_cameraData.znear = camera->ZNear();
    m_cameraData.zfar = camera->ZFar();
}

void CameraSystem::UploadEditorCameraData()
{
    m_cameraData.viewMatrix = m_editorCamera->View();
    m_cameraData.inverseViewMatrix = glm::inverse(m_cameraData.viewMatrix);
    m_cameraData.projectionMatrix = m_editorCamera->Projection();
    m_cameraData.inverseProjectionMatrix = glm::inverse(m_cameraData.projectionMatrix);
    m_cameraData.position = m_editorCamera->Position();
    m_cameraData.znear = m_editorCamera->ZNear();
    m_cameraData.zfar = m_editorCamera->ZFar();
}

void CameraSystem::OnEvent(const std::shared_ptr<evt::IEvent> &e)
{
    if (e->Type() == evt::EventType::FramebufferResized)
    {
        auto event = std::dynamic_pointer_cast<evt::FramebufferResizedEvent>(e);
        m_viewportWidth = static_cast<uint32_t>(event->m_size.x);
        m_viewportHeight = static_cast<uint32_t>(event->m_size.y);
    }
}
} // lum